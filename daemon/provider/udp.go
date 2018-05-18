package provider

import (
	"fmt"
	"net"
	"strings"
	"time"
)

type Conn interface {
	ReadFrom(b []byte) (n int, addr net.Addr, err error)
	WriteTo(b []byte, addr net.Addr) (n int, err error)
}

const (
	PROVIDER_PORT    = "9988"
	UDP_COMM_BUF_LEN = 256
)

///////////////////////////////////////////////////////////
func getLocalIPv4Addr() string {
	addr, _ := net.InterfaceAddrs()

	//ipv4 addr
	for _, a := range addr {
		e := strings.Index(a.String(), "/24")
		//fmt.Println(a)
		if e != -1 {
			return a.String()[:e]
		}
	}

	//loop addr
	for _, a := range addr {
		e := strings.Index(a.String(), "/8")
		//fmt.Println(a)
		if e != -1 {
			return a.String()[:e]
		}
	}
	return ""
}

func getLocalIPv4BroadcastAddr() string {
	ip := getLocalIPv4Addr()
	if ip != "127.0.0.1" {
		e := strings.LastIndex(ip, ".")
		if e != -1 {
			return ip[:e+1] + "255"
		} else {
			return ""
		}
	} else {
		return "127.0.0.1"
	}

}

//////////////////////////////////////////////////////////
type UDPServer struct {
	conn    net.PacketConn
	srcAddr net.Addr
}

func GetUDPServer() (*UDPServer, error) {
	u := &UDPServer{}
	conn, err := net.ListenPacket("udp4", ":"+PROVIDER_PORT)
	if err != nil {
		return nil, err
	} else {
		u.conn = conn
		return u, nil
	}
}

func ReadFrom(conn Conn, data []byte) (int, net.Addr, error) {
	rNeedLen := len(data)
	rLen := 0
	l := 0
	var addr net.Addr
	var err error
	for {
		l, addr, err = conn.ReadFrom(data[rLen:])
		if err != nil {
			fmt.Println("UDP read fail:", err.Error())
			rLen = 0
			break
		} else {
			rLen += l
			if rLen >= rNeedLen {
				break
			}
		}
	}

	return rLen, addr, err
}

func WriteTo(conn Conn, data []byte, addr net.Addr) (int, error) {
	wNeedLen := len(data)
	wLen := 0
	l := 0

	var err error
	for {
		l, err = conn.WriteTo(data[wLen:], addr)
		if err != nil {
			fmt.Println("UDP read fail:", err.Error())
			wLen = 0
			break
		} else {
			wLen += l
			if wLen >= wNeedLen {
				break
			}
		}
	}

	return wLen, err
}

func (u *UDPServer) Read(data []byte) (n int, err error) {
	n, u.srcAddr, err = ReadFrom(u.conn, data)
	return
}

func (u *UDPServer) Write(data []byte) (n int, err error) {
	return WriteTo(u.conn, data, u.srcAddr)
}

/////////////////////////////////////////////////

type UDPClient struct {
	conn    *net.UDPConn
	srcAddr *net.UDPAddr
	dstAddr *net.UDPAddr
}

func GetUDPClient() (*UDPClient, error) {
	u := &UDPClient{}

	//change to ipv4 addr
	u.srcAddr, _ = net.ResolveUDPAddr("udp4", getLocalIPv4Addr()+":0")
	u.dstAddr, _ = net.ResolveUDPAddr("udp4", getLocalIPv4BroadcastAddr()+":"+PROVIDER_PORT)

	conn, err := net.ListenUDP("udp4", u.srcAddr)
	if err != nil {
		return nil, err
	} else {
		conn.SetReadDeadline(time.Now().Add(time.Millisecond * 500))
		u.conn = conn
		return u, nil
	}
}

func (u *UDPClient) Read(data []byte) (n int, err error) {
	n, _, err = ReadFrom(u.conn, data)
	return
}

func (u *UDPClient) Write(data []byte) (n int, err error) {
	return WriteTo(u.conn, data, u.dstAddr)
}
