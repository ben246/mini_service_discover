package provider

import (
	"encoding/json"
	"fmt"
	"unsafe"
)

type ServiceHead struct {
	Head     uint16
	Action   ActionType
	JsonLen  uint16
	CheckSum uint16
}

var emptyServiceHead ServiceHead
var ServiceHeadLen = int(unsafe.Sizeof(emptyServiceHead))

type ServiceInfo struct {
	DeviceName  string `json:"device_name"`
	ServiceName string `json:"service_name"`
	IP          string `json:"ip"`
	Port        uint16 `json:"port"`
	Info        string `json:"info"`
}

type ResultInfo struct {
	Reason string `json:"reason"`
}

const (
	SERVICE_NO_FOUND = "service no found"
	REGISTER_SUCC    = "register succ"
)

type ActionType uint16

const (
	INFO_TYPE_REGISTER_SERVICE ActionType = iota
	INFO_TYPE_UNREGISTER_SERVICE
	INFO_TYPE_INQUIRY
	INFO_TYPE_ACTION_SUCC
	INFO_TYPE_ACTION_FAIL

	INFO_HEAD = 0xFFEE

	INFO_MAX_LEN = 100
)

type sliceMock struct {
	addr uintptr
	len  int
	cap  int
}

func ServiceHeadToBytes(head *ServiceHead) []byte {
	Len := unsafe.Sizeof(*head)
	tmp := &sliceMock{
		addr: uintptr(unsafe.Pointer(head)),
		cap:  int(Len),
		len:  int(Len),
	}
	return *(*[]byte)(unsafe.Pointer(tmp))
}

func BytesToServiceHead(data []byte) *ServiceHead {
	h := data[:ServiceHeadLen]
	return *(**ServiceHead)(unsafe.Pointer(&h))
}

func getJson(data []byte) []byte {
	end := 0
	for i, d := range data {
		if string(d) == "}" {
			end = i + 1
		}
	}
	if end > UDP_COMM_BUF_LEN {
		end = UDP_COMM_BUF_LEN
	}

	return data[:end]
}

func PrintData(data []byte) {
	head := BytesToServiceHead(data)
	fmt.Println("Head", head.Head)
	fmt.Println("Action", head.Action)
	fmt.Println("JsonLen", head.JsonLen)
	fmt.Println("CheckSum", head.CheckSum)

	fmt.Println(string(data[ServiceHeadLen:]))
}

func PrintServiceInfoData(data []byte) {
	fmt.Println(string(data[ServiceHeadLen:]))
}

func CheckAndGetServiceInfoHead(data []byte) *ServiceHead {
	h := BytesToServiceHead(data)
	if h.Head != INFO_HEAD {
		return nil
	}

	d := data[ServiceHeadLen : ServiceHeadLen+int(h.JsonLen)]

	var sum uint16
	for _, u := range d {
		sum += uint16(u)
	}

	if sum == h.CheckSum {
		return h
	} else {
		return nil
	}
}

func GetServiceInfo(data []byte) *ServiceInfo {
	info := &ServiceInfo{}
	err := json.Unmarshal(getJson(data), info) //Unmarshal need the pointer of struct
	if err != nil {
		fmt.Println("GetServiceInfo fail:", err.Error())
		return nil
	} else {
		return info
	}
}

func GetServiceHead(action ActionType, data []byte) []byte {
	h := &ServiceHead{
		Head:    INFO_HEAD,
		Action:  action,
		JsonLen: uint16(len(data)),
	}

	for _, d := range data {
		h.CheckSum += uint16(d)
	}

	return ServiceHeadToBytes(h)
}

//action
func ServiceInfoToBytes(info *ServiceInfo) []byte {
	d, err := json.Marshal(*info) //Marshal need struct self
	if err != nil {
		fmt.Println(err.Error())
		return nil
	} else {
		return d
	}
}

func GetServiceData(action ActionType, s *ServiceInfo) []byte {
	info := ServiceInfoToBytes(s)
	if info != nil {
		h := GetServiceHead(action, info)
		hLen := len(h)
		data := make([]byte, UDP_COMM_BUF_LEN) //match daemon read len
		copy(data, h)
		copy(data[hLen:], info)
		return data
	} else {
		return nil
	}
}

func getValidStr(str string) string {
	if len(str) > INFO_MAX_LEN {
		return str[:INFO_MAX_LEN]
	} else {
		return str
	}
}

func GetRegisterService(devName, serviceName, ip string, port uint16, info string) []byte {
	s := &ServiceInfo{
		DeviceName:  getValidStr(devName),
		ServiceName: getValidStr(serviceName),
		IP:          getValidStr(ip),
		Port:        port,
		Info:        getValidStr(info),
	}
	return GetServiceData(INFO_TYPE_REGISTER_SERVICE, s)
}

func GetUnRegisterService(devName, serviceName string) []byte {
	s := &ServiceInfo{
		DeviceName:  getValidStr(devName),
		ServiceName: getValidStr(serviceName),
	}
	return GetServiceData(INFO_TYPE_UNREGISTER_SERVICE, s)
}

func GetInquiryService(servieName string) []byte {
	s := &ServiceInfo{
		ServiceName: getValidStr(servieName),
	}
	return GetServiceData(INFO_TYPE_INQUIRY, s)
}

//result
func ResultInfoToBytes(info *ResultInfo) []byte {
	d, err := json.Marshal(*info) //Marshal need struct self
	if err != nil {
		fmt.Println(err.Error())
		return nil
	} else {
		return d
	}
}

func GetResultData(action ActionType, s *ResultInfo) []byte {
	info := ResultInfoToBytes(s)
	if info != nil {
		h := GetServiceHead(action, info)
		hLen := len(h)
		data := make([]byte, UDP_COMM_BUF_LEN) //match daemon read len
		copy(data, h)
		copy(data[hLen:], info)
		return data
	} else {
		return nil
	}
}

func GetResult(action ActionType, reason string) []byte {
	s := &ResultInfo{
		Reason: getValidStr(reason),
	}
	return GetResultData(action, s)
}

func GetResultInfo(data []byte) *ResultInfo {
	info := &ResultInfo{}
	err := json.Unmarshal(getJson(data), info) //Unmarshal need the pointer of struct
	if err != nil {
		fmt.Println("GetResultInfo fail:", err.Error())
		return nil
	} else {
		return info
	}
}
