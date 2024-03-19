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

func Task28_29() {
    // Получить URL из командной строки
    if len(os.Args) != 2 {
        fmt.Println("Использование:", os.Args[0], "<URL>")
        return
    }
    url := os.Args[1]

    // Создать HTTP-клиент
    client := http.Client{}

    // Создать запрос
    req, err := http.NewRequest("GET", url, nil)
    if err != nil {
        fmt.Println("Ошибка создания запроса:", err)
        return
    }

    // Отправить запрос
    resp, err := client.Do(req)
    if err != nil {
        fmt.Println("Ошибка выполнения запроса:", err)
        return
    }

    // Обработать ответ
    defer resp.Body.Close()
    body, err := io.ReadAll(resp.Body)
    if err != nil {
        fmt.Println("Ошибка чтения ответа:", err)
        return
    }

    // Вывести тело ответа
		lines := strings.Split(string(body), "\n")
		err2 := termbox.Init()
		if err2 != nil {
			fmt.Println("Error initializing termbox:", err)
			os.Exit(1)
		}
		defer termbox.Close()

		for i, line := range lines {
			fmt.Println(line)

			if i >= 25 {
				// Wait for SPACE key press using termbox
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
}