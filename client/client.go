package main

import (
	"fmt"
	. "mini_service_discover/daemon/provider"
	"time"
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
		d.InquiryService()
	}
}

func (d *ClientData) InquiryService() {
	q := GetInquiryService("play_music")
	d.client.Write(q)
	//fmt.Println("INFO_TYPE_REGISTER_SERVICE", INFO_TYPE_REGISTER_SERVICE)
	//PrintData(q)
	for {
		info := d.GetResult()
		if info == nil {
			break
		}
		fmt.Println("get service:", info.DeviceName, ",", info.ServiceName, ",", info.IP, ",", info.Port)
	}

	for {
		time.Sleep(time.Second)
		//fmt.Println("sleep 1 sec")
	}
}

func (d *ClientData) GetResult() *ServiceInfo {
	data := make([]byte, UDP_COMM_BUF_LEN)

	_, err := d.client.Read(data)
	if err != nil {
		return nil
	}

	head := CheckAndGetServiceInfoHead(data)
	if head == nil {
		fmt.Println("ProcessData checksum fail")
		return nil
	}

	if head.Action == INFO_TYPE_ACTION_SUCC {
		return GetServiceInfo(data[ServiceHeadLen:])
	} else {
		info := GetResultInfo(data[ServiceHeadLen:])
		if info != nil {
			fmt.Println("get service fail:", info.Reason)
		}
		return nil
	}
}
