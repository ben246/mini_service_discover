package main

import (
	"fmt"
	. "mini_service_discover/daemon/provider"
)

type ClientData struct {
	client *UDPClient
}

func GetClientData() *ClientData {
	client, err := GetUDPClient()
	if err != nil {
		fmt.Println("GetUDPClient fail:", err.Error())
		return nil
	}

	d := &ClientData{
		client: client,
	}

	return d
}

func main() {
	d := GetClientData()
	if d != nil {
		d.RegisterService()
	}
}

func (d *ClientData) RegisterService() {
	d1 := GetRegisterService("box1", "play_music", "192.168.1.116", 6666)
	//PrintData(d1)
	d.client.Write(d1)
	d.GetResult()

	d2 := GetRegisterService("box2", "play_music", "192.168.1.116", 6666)
	d.client.Write(d2)
	d.GetResult()
}

func (d *ClientData) GetResult() (succ bool) {
	data := make([]byte, UDP_COMM_BUF_LEN)

	_, err := d.client.Read(data)
	if err != nil {
		return false
	}

	head := CheckAndGetServiceInfoHead(data)
	if head == nil {
		fmt.Println("ProcessData checksum fail")
		return false
	}

	if head.Action == INFO_TYPE_ACTION_SUCC {
		return true
	} else {
		info := GetResultInfo(data[ServiceHeadLen:])
		if info != nil {
			fmt.Println("get service fail:", info.Reason)
		}
		return false
	}
}
