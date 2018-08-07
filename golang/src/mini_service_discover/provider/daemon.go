package provider

import (
	"fmt"
	. "mini_service_discover/provider/comm"
)

type DaemonData struct {
	server   *UDPServer
	provider *Provider
}

func GetDaemonData() *DaemonData {
	s, err := GetUDPServer()
	if err != nil {
		fmt.Println("GetUDPServer fail:", err.Error())
		return nil
	}

	d := &DaemonData{
		server:   s,
		provider: GetProvider(),
	}

	return d
}

func (d *DaemonData) Loop() {
	buf := make([]byte, UDP_COMM_BUF_LEN)
	s := d.server

	for {
		_, err := s.Read(buf)
		if err != nil {
			continue
		} else {
			d.ProcessData(buf)
		}
	}
}

func (d *DaemonData) ProcessData(data []byte) {
	head := CheckAndGetServiceInfoHead(data)
	if head == nil {
		fmt.Println("ProcessData checksum fail")
		return
	}
	//fmt.Println(head)

	info := GetServiceInfo(data[ServiceHeadLen:])
	if info != nil {
		var err error

		switch head.Action {
		case INFO_TYPE_REGISTER_SERVICE:
			fmt.Println("server action:", "register", info.DeviceName, ",", info.ServiceName, ",", info.IP, ",", info.Port, info.Info)
			d.provider.AddInfo(info)
			data := GetResult(INFO_TYPE_ACTION_SUCC, "")
			_, err = d.server.Write(data)
			fmt.Println(string(data[ServiceHeadLen:]))
			break
		case INFO_TYPE_UNREGISTER_SERVICE:
			fmt.Println("server action:", "unregister", info.DeviceName, ",", info.ServiceName)
			if d.provider.RemoveByName(info.DeviceName, info.ServiceName) {
				data := GetResult(INFO_TYPE_ACTION_SUCC, "")
				_, err = d.server.Write(data)
				fmt.Println(string(data[ServiceHeadLen:]))
			} else {
				data := GetResult(INFO_TYPE_ACTION_FAIL, SERVICE_NO_FOUND)
				_, err = d.server.Write(data)
				fmt.Println(string(data[ServiceHeadLen:]))
			}
			break
		case INFO_TYPE_INQUIRY:
			fmt.Println("server action:", "inquiry", info.ServiceName)
			c := d.provider.GetCount(info.ServiceName)
			if c > 0 {
				res := make([]*ServiceInfo, c)
				d.provider.GetAll(info.ServiceName, res)
				for _, i := range res {
					data := GetServiceData(INFO_TYPE_ACTION_SUCC, i)
					_, err = d.server.Write(data)
				}
			} else {
				data := GetResult(INFO_TYPE_ACTION_FAIL, SERVICE_NO_FOUND)
				_, err = d.server.Write(data)
			}
		default:
			fmt.Println("GetServiceInfo unknow action", head.Action)
		}

		if err != nil {
			fmt.Println(err.Error())
		}
	}

}
