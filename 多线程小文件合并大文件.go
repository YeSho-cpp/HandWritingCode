package main

import (
    "bufio"
    "fmt"
    "io"
    "os"
    "path/filepath"
    "sync"
)


// todo 难点1 我要怎么读取一行

func read_line(files []string,outputPath *os.File){
	for _,file:=range files{
			f,_:=os.Open(file)
			reader:=bufio.NewReader(f)

			line,_:=reader.ReadString('\n')

			outputPath.WriteString(line)
	}
}

// todo 难点2 我怎么遍历某个目录下的所有相关文件

func get_files(files []string,intputPath string){
	filepath.Walk(intputPath,func (path string,fifo os.FileInfo,err error) error {
			if err!=nil{
					return err
			}
			if !fifo.IsDir()&&filepath.Ext(path)==".txt"{
					files=append(files, path)
			}
			return nil
	})
}



// 工作协程处理文件
func worker(files []string, outputFile *os.File, mu *sync.Mutex, wg *sync.WaitGroup) {
    defer wg.Done()

    for _, file := range files {
        // 打开并读取文件
        f, err := os.Open(file)
        if err != nil {
            fmt.Printf("Error opening file %s: %v\n", file, err)
            continue
        }

        reader := bufio.NewReader(f)
        for {
            line, err := reader.ReadString('\n')
            if err != nil {
                if err != io.EOF {
                    fmt.Printf("Error reading file %s: %v\n", file, err)
                }
                break
            }

            // 使用互斥锁保护写操作
            mu.Lock()
            _, err = outputFile.WriteString(line)
            mu.Unlock()

            if err != nil {
                fmt.Printf("Error writing to output file: %v\n", err)
                f.Close()
                return
            }
        }
        f.Close()
    }
}

func main() {
    // 指定输入目录和输出文件
    inputDir := "./input"  // 包含txt文件的目录
    outputPath := "./output.txt"

    // 创建输出文件
    outputFile, err := os.Create(outputPath)
    if err != nil {
        fmt.Printf("Error creating output file: %v\n", err)
        return
    }
    defer outputFile.Close()

    // 获取所有txt文件
    var files []string
    err = filepath.Walk(inputDir, func(path string, info os.FileInfo, err error) error {
        if err != nil {
            return err
        }
        if !info.IsDir() && filepath.Ext(path) == ".txt" {
            files = append(files, path)
        }
        return nil
    })
    if err != nil {
        fmt.Printf("Error walking directory: %v\n", err)
        return
    }

    // 设置工作协程数量
    numWorkers := 4
    filesPerWorker := len(files) / numWorkers
    var wg sync.WaitGroup
    mu := &sync.Mutex{}

    // 启动工作协程
    for i := 0; i < numWorkers; i++ {
        start := i * filesPerWorker
        end := start + filesPerWorker
        if i == numWorkers-1 {
            end = len(files) // 确保处理所有剩余文件
        }

        wg.Add(1)
        go worker(files[start:end], outputFile, mu, &wg)
    }

    // 等待所有工作协程完成
    wg.Wait()
    fmt.Println("文件合并完成")
}