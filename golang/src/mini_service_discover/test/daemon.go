package main

import (
	. "mini_service_discover/provider"
)

func main() {
	d := GetDaemonData()
	if d != nil {
		d.Loop()
	}
}
