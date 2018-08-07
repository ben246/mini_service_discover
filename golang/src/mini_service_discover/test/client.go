package main

import (
	"fmt"
	. "mini_service_discover/provider"
)

func main() {
	c := GetClientData()
	if c == nil {
		fmt.Println("get client data fail")
	}

	infos := c.InquiryService("play_music")
	for _, i := range infos {
		devName, ip, port, info := i.DeviceName, i.IP, i.Port, i.Info
		fmt.Println("play_music:", devName, ip, port, info)
	}
	c.Quit()
}
