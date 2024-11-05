package main


import "fmt"

type Server struct{
	name string
	weight int
}

type LoadBanlancer struct {
	virtualNodes []string
	current int
	servers map[string]int
}

func NewLoadBalancer(servers map[string]int) *LoadBanlancer{
	servers=servers


	var nodes []string
	for serverName,wight :=range servers{
		for i:=0;i<wight;i++{
			nodes=append(nodes, serverName)
		}
	}

	return &LoadBanlancer{
		virtualNodes: nodes,
		current: -1,
	}
}

func (lb *LoadBanlancer)GetServer() string{
	if len(lb.virtualNodes)==0{
		return ""
	}

	lb.current=(lb.current+1)%len(lb.virtualNodes)
	return lb.virtualNodes[lb.current]
}


func main(){

	servers := map[string]int{
		"A": 5,
		"B": 3,
		"C": 2,
	}

	lb:=NewLoadBalancer(servers)

	for i:=0;i<20;i++{
		fmt.Printf("Request %d goes to: %s\n",i+1,lb.GetServer())
	}
}