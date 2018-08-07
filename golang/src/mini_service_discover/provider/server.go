package provider

import (
	"fmt"
	. "mini_service_discover/provider/comm"
	"strings"
)

type ServerData struct {
	client        *UDPClient
	recEnd        bool
	broadcastQuit chan bool
}

func GetServerData() *ServerData {
	//client, err := GetUDPClient(true)
	client, err := GetUDPClient(false)
	if err != nil {
		fmt.Println("GetUDPClient fail:", err.Error())
		return nil
	}

	d := &ServerData{
		client:        client,
		broadcastQuit: make(chan bool, 1),
	}

	return d
}

func (d *ServerData) Quit() {
	close(d.broadcastQuit)
}

func (d *ServerData) RegisterService(devName, serviceName, ip string, port uint16, info string) (succ bool, failReason string) {
	data := GetRegisterService(devName, serviceName, ip, port, info)
	return d.GetResult(data)
}

func (d *ServerData) UnRegisterService(devName, serviceName string) (succ bool, failReason string) {
	data := GetUnRegisterService(devName, serviceName)
	return d.GetResult(data)
}

func (d *ServerData) GetResult(sendData []byte) (succ bool, failReason string) {
	data := make([]byte, UDP_COMM_BUF_LEN)

	rec := false

	if d.client.IsBroadcast() {
		d.client.Write(sendData)
		_, err := d.client.Read(data)
		if err != nil {
			fmt.Println(err.Error())
			return false, err.Error()
		}
		rec = true
	} else {

		go d.UDPBroadcastData(sendData)

		d.recEnd = false
		n, err := d.client.Read(data)
		if err != nil &&
			!strings.Contains(err.Error(), "timeout") {
			fmt.Println(err.Error())
		}
		d.recEnd = true
		<-d.broadcastQuit

		if n > 0 {
			rec = true
		}
	}

	if !rec {
		//fmt.Println("read nothing")
		return false, "read nothing"
	}

	head := CheckAndGetServiceInfoHead(data)
	if head == nil {
		//fmt.Println("ProcessData checksum fail")
		return false, "ProcessData checksum fail"
	}

	if head.Action == INFO_TYPE_ACTION_SUCC {
		//fmt.Println(string(data[ServiceHeadLen:]))
		//fmt.Println("succ")
		succ = true
	} else {
		info := GetResultInfo(data[ServiceHeadLen:])
		if info != nil {
			//fmt.Println("action fail:", info.Reason)
			failReason = info.Reason
		}
	}

	return
}

func (d *ServerData) UDPBroadcastData(data []byte) {
	finish := false
	d.client.ResetSubAddr()

	for !finish {
		if d.recEnd {
			break
		}
		finish = d.client.PrepareSubAddr()
		d.client.WriteSub(data)
	}

	d.broadcastQuit <- true
}
