package provider

import (
	"fmt"
	. "mini_service_discover/provider/comm"
	"strings"
)

type ClientData struct {
	client        *UDPClient
	recEnd        bool
	broadcastQuit chan bool
}

func GetClientData() *ClientData {
	//client, err := GetUDPClient(true)
	client, err := GetUDPClient(false)
	if err != nil {
		fmt.Println("GetUDPClient fail:", err.Error())
		return nil
	}

	d := &ClientData{
		client:        client,
		broadcastQuit: make(chan bool, 1),
	}

	return d
}

func (d *ClientData) Quit() {
	close(d.broadcastQuit)
}

func (d *ClientData) InquiryService(serviceName string) []ServiceInfo {
	return d.GetInquiryResult(serviceName)
}

func (d *ClientData) GetInquiryResult(serviceName string) []ServiceInfo {
	data := make([]byte, UDP_COMM_BUF_LEN)
	var infos []ServiceInfo

	sendData := GetInquiryService(serviceName)

	if d.client.IsBroadcast() {
		d.client.Write(sendData)
	} else {
		go d.UDPBroadcastData(sendData)
	}

	d.recEnd = false
	for {
		n, err := d.client.Read(data)
		if err != nil &&
			!strings.Contains(err.Error(), "timeout") {
			fmt.Println(err.Error())
		}
		d.recEnd = true

		if n > 0 {
			head := CheckAndGetServiceInfoHead(data)
			if head == nil {
				fmt.Println("ProcessData checksum fail")
				break
			}

			if head.Action == INFO_TYPE_ACTION_SUCC {
				i := GetServiceInfo(data[ServiceHeadLen:])
				infos = append(infos, *i)
			} else {
				info := GetResultInfo(data[ServiceHeadLen:])
				if info != nil {
					fmt.Println("get service fail:", info.Reason)
				}
				break
			}
			continue
		}
		break
	}

	if !d.client.IsBroadcast() {
		<-d.broadcastQuit
	}
	return infos
}

func (d *ClientData) UDPBroadcastData(data []byte) {
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
