package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"os"
)


func readWholeFile(){
	data,err:=os.ReadFile("./in.out")
	if err!=nil{
		log.Fatal(err)
	}
	fmt.Println(string(data))
}

func readByBlock(){
	f,err:=os.Open("./in.out")

	if err!=nil{
		log.Fatal(err)
	}

	defer f.Close()

	buffer:=make([]byte, 1024)

	for{
		n,err:=f.Read(buffer)
		if err==io.EOF{
			break
		}
		fmt.Println(string(buffer[:n]))
	}
}


func readByLine(){
	f,err:=os.Open("./in.out")
	if err!=nil{
		log.Fatal(err)
	}

	defer f.Close()

	scanner :=bufio.NewScanner(f)

	for scanner.Scan(){
		fmt.Println(scanner.Text())
	}
}


func writeWholeFile(){
	data:=[]byte("Hello World!")
	err:=os.WriteFile("test.txt",data,0644)

	if err!=nil{
		log.Fatal(err)
	}
}

func appendToFile(){
	f,err:=os.OpenFile("test.txt",os.O_APPEND|os.O_CREATE|os.O_WRONLY,0644)
	if err!=nil{
		log.Fatal(err)
	}

	defer f.Close()

	if _,err:=f.WriteString("新内容\n");err!=nil{
		log.Fatal(err)
	}
}

func main(){
	// readWholeFile()
	// readByBlock()

  readByLine()
}