package main

import (
	"fmt"
	. "mini_service_discover/provider"
	"time"
)

func main() {
	c := GetServerData()
	if c == nil {
		fmt.Println("get client data fail")
	}

	fmt.Println("register box1, play_music")
	succ, rea := c.RegisterService("box1", "play_music", "192.168.1.116", 6666, "play music with pcm data fskjdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddfklsaaaaaaaaaaaaaaa")
	if !succ {
		fmt.Println(rea)
	} else {
		fmt.Println("succ")
	}

	fmt.Println("register box2, play_music")
	succ, rea = c.RegisterService("box2", "play_music", "192.168.1.116", 6666, "play music with acc data")
	if !succ {
		fmt.Println(rea)
	} else {
		fmt.Println("succ")
	}

	time.Sleep(time.Second * 6)

	fmt.Println("unregister box2, play_music")
	succ, rea = c.UnRegisterService("box2", "play_music")
	if !succ {
		fmt.Println(rea)
	} else {
		fmt.Println("succ")
	}

	time.Sleep(time.Second * 3)

	c.Quit()
}
