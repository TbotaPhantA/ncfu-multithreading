package main

import (
	"fmt"
	"io"
	"net/http"
	"os"
	"strings"
	"time"
	"github.com/nsf/termbox-go"
)

func Task30() {
    // Получение URL из командной строки
    if len(os.Args) != 2 {
        fmt.Println("Использование:", os.Args[0], "<url>")
        return
    }
    url := os.Args[1]

    // Создание каналов для передачи данных
    dataCh := make(chan []byte)
    doneCh := make(chan bool)

    // Запуск потока для получения данных
    go func() {
        // Отправка запроса
        resp, err := http.Get(url)
        if err != nil {
            fmt.Println("Ошибка:", err)
            doneCh <- true
            return
        }

        // Чтение ответа
        defer resp.Body.Close()
        buf := make([]byte, 65536)
        for {
            n, err := resp.Body.Read(buf)
            if err != nil {
                if err != io.EOF {
                    fmt.Println("Ошибка:", err)
                }
                break
            }
            dataCh <- buf[:n]
        }

        doneCh <- true
    }()

    // Запуск потока для вывода данных
    go func() {
        // Счетчик строк
        lines := 0

        // Цикл обработки данных
        for {
            select {
            case data := <-dataCh:
                // Вывод данных
								err2 := termbox.Init()
								if err2 != nil {
									fmt.Println("Error initializing termbox:", err2)
									os.Exit(1)
								}
								defer termbox.Close()
                for _, line := range strings.Split(string(data), "\n") {
                    lines++
                    fmt.Println(line)
										if (lines >= 25) {
											event := termbox.PollEvent()
											for event.Type != termbox.EventKey {
												event = termbox.PollEvent()
											}

											if event.Key != termbox.KeySpace {
												fmt.Println("You pressed a different key. Please press SPACE.")
												// Wait for another key press (optional)
												time.Sleep(time.Second)
											}
										}
                }
            case <-doneCh:
                // Завершение работы
                return
            }
        }
    }()

    // Ожидание завершения работы
    <-doneCh
}
