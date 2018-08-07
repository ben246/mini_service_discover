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
	PROVIDER_PORT      = "9988"
	UDP_COMM_BUF_LEN   = 1024
	UDP_SOCKET_TIMEOUT = 500 //millisecond
	LOCAL_LOOP_ADDR    = "127.0.0.1"
)

///////////////////////////////////////////////////////////
func GetMillliSecond() int64 {
	return time.Now().UnixNano() / 1e6
}

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

func getLocalIPv4Network() string {
	ip := getLocalIPv4Addr()
	if ip != LOCAL_LOOP_ADDR {
		e := strings.LastIndex(ip, ".")
		if e != -1 {
			return ip[:e+1]
		} else {
			return ""
		}
	} else {
		return LOCAL_LOOP_ADDR
	}

}

func getLocalIPv4BroadcastAddr() string {
	ip := getLocalIPv4Addr()
	if ip != LOCAL_LOOP_ADDR {
		e := strings.LastIndex(ip, ".")
		if e != -1 {
			return ip[:e+1] + "255"
		} else {
			return ""
		}
	} else {
		return LOCAL_LOOP_ADDR
	}

}

func ReadFrom(conn *net.UDPConn, data []byte) (int, net.Addr, error) {
	rNeedLen := len(data)
	rLen := 0
	l := 0
	var addr net.Addr
	var err error
	for {
		l, addr, err = conn.ReadFrom(data[rLen:])
		//fmt.Println(l)
		if err != nil {
			if !strings.Contains(err.Error(), "timeout") {
				fmt.Println("UDP ReadFrom fail:", err.Error())
			}
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

func WriteTo(conn *net.UDPConn, data []byte, addr net.Addr) (int, error) {
	wNeedLen := len(data)
	wLen := 0
	l := 0

	var err error
	for {
		l, err = conn.WriteTo(data[wLen:], addr)
		if err != nil {
			fmt.Println("UDP WriteTo fail:", err.Error())
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

func Read(conn *net.UDPConn, data []byte) (int, error) {
	rNeedLen := len(data)
	rLen := 0
	l := 0
	var err error
	for {
		l, err = conn.Read(data[rLen:])
		//fmt.Println(l)
		if err != nil {
			if !strings.Contains(err.Error(), "timeout") {
				fmt.Println("UDP ReadFrom fail:", err.Error())
			}
			rLen = 0
			break
		} else {
			rLen += l
			if rLen >= rNeedLen {
				break
			}
		}
	}

	return rLen, err
}

func Write(conn *net.UDPConn, data []byte) (int, error) {
	wNeedLen := len(data)
	wLen := 0
	l := 0

	var err error
	for {
		l, err = conn.Write(data[wLen:])
		if err != nil {
			fmt.Println("UDP Write fail:", err.Error())
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

//////////////////////////////////////////////////////////
type UDPServer struct {
	conn    *net.UDPConn
	srcAddr net.Addr
}

func GetUDPServer() (*UDPServer, error) {
	u := &UDPServer{}
	addr, _ := net.ResolveUDPAddr("udp4", ":"+PROVIDER_PORT)
	conn, err := net.ListenUDP("udp4", addr)
	if err != nil {
		return nil, err
	} else {
		u.conn = conn
		return u, nil
	}
}

func (u *UDPServer) Read(data []byte) (n int, err error) {
	n, u.srcAddr, err = ReadFrom(u.conn, data)
	//fmt.Println(u.srcAddr.String())
	return
}

func (u *UDPServer) Write(data []byte) (n int, err error) {
	n, err = WriteTo(u.conn, data, u.srcAddr)
	return
}

/////////////////////////////////////////////////

type UDPClient struct {
	conn           *net.UDPConn
	srcAddr        *net.UDPAddr
	dstAddr        *net.UDPAddr
	broadcastValid bool
	addrNetwork    string
	subIP          int
}

func GetUDPClient(useBroadcast bool) (u *UDPClient, err error) {
	u = &UDPClient{}

	//change to ipv4 addr
	u.srcAddr, _ = net.ResolveUDPAddr("udp4", ":0")

	if useBroadcast || getLocalIPv4Addr() == LOCAL_LOOP_ADDR {
		u.broadcastValid = true
		u.dstAddr, _ = net.ResolveUDPAddr("udp4", getLocalIPv4BroadcastAddr()+":"+PROVIDER_PORT)
	} else {
		u.addrNetwork = getLocalIPv4Network()
		u.broadcastValid = false
		u.subIP = 1
	}

	conn, err := net.ListenUDP("udp4", u.srcAddr)
	if err != nil {
		fmt.Println(err.Error())
		return nil, err
	} else {
		u.conn = conn
		return u, nil
	}
	return u, nil
}

func (u *UDPClient) IsBroadcast() bool {
	return u.broadcastValid
}

func (u *UDPClient) Read(data []byte) (n int, err error) {
	u.conn.SetReadDeadline(time.Now().Add(time.Millisecond * UDP_SOCKET_TIMEOUT))
	n, err = Read(u.conn, data)
	return
}

func (u *UDPClient) Write(data []byte) (n int, err error) {
	return WriteTo(u.conn, data, u.dstAddr)
}

func (u *UDPClient) ResetSubAddr() {
	u.subIP = 1
}

func (u *UDPClient) PrepareSubAddr() (finish bool) {
	ip := u.addrNetwork + fmt.Sprintf("%d", u.subIP)
	//fmt.Println(ip)
	u.dstAddr, _ = net.ResolveUDPAddr("udp4", ip+":"+PROVIDER_PORT)

	if u.subIP < 254 {
		u.subIP++
	} else {
		finish = true
	}
	return
}

func (u *UDPClient) WriteSub(data []byte) (n int, err error) {
	n, err = u.Write(data)
	if err != nil {
		fmt.Println(err.Error())
	}
	return
}
