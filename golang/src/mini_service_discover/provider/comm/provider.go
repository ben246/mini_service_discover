package provider

import (
	"container/list"
	//"fmt"
)

type Provider struct {
	list *list.List
}

func GetProvider() *Provider {
	return &Provider{
		list: list.New(),
	}
}

func (p *Provider) AddInfo(i *ServiceInfo) {
	//remove the same service
	p.RemoveByName(i.DeviceName, i.ServiceName)
	p.list.PushFront(i)
}

func (p *Provider) Add(devName, serviceName, ip string, port uint16) {
	i := &ServiceInfo{
		DeviceName:  devName,
		ServiceName: serviceName,
		IP:          ip,
		Port:        port,
	}
	p.AddInfo(i)
}

func (p *Provider) RemoveByName(devName, serviceName string) bool {
	l := p.list
	for e := l.Front(); e != nil; e = e.Next() {
		info := e.Value.(*ServiceInfo)
		if info != nil &&
			info.DeviceName == devName &&
			info.ServiceName == serviceName {
			l.Remove(e)
			return true
		}
	}

	return false
}

func (p *Provider) Get(serviceName string) *ServiceInfo {
	l := p.list
	for e := l.Front(); e != nil; e = e.Next() {
		info := e.Value.(*ServiceInfo)
		if info != nil &&
			info.ServiceName == serviceName {
			return info
		}
	}
	return nil
}

func (p *Provider) GetCount(serviceName string) int {
	l := p.list
	i := 0
	for e := l.Front(); e != nil; e = e.Next() {
		info := e.Value.(*ServiceInfo)
		if info != nil &&
			info.ServiceName == serviceName {
			i++
		}
	}
	return i
}

func (p *Provider) GetAll(serviceName string, infoOut []*ServiceInfo) {
	l := p.list
	i := 0
	size := len(infoOut)
	for e := l.Front(); e != nil; e = e.Next() {
		info := e.Value.(*ServiceInfo)
		if info != nil &&
			info.ServiceName == serviceName {
			if i < size {
				infoOut[i] = info
			}
		}
		i++
	}
}
