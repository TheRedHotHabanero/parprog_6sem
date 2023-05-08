# Параллельное программирование
## 6 семестр, Лирисман Карина, Б01-008

<image src="flowers1.jpg">

# Вводные задачи по MPI
* [Условия задач](https://docs.yandex.ru/docs/view?url=ya-disk-public%3A%2F%2FWWP6MJYY23lknWYddiCjug8F2Ir9qyNSu57Ba5zP2OUR%2F%2FW55cFQjKU7UcdCNOWSRmR%2F0fePyGwwW%2FWKW0%2FCEA%3D%3D%3A%2F%D0%92%D0%B2%D0%BE%D0%B4%D0%BD%D1%8B%D0%B5%20%D0%B7%D0%B0%D0%B4%D0%B0%D1%87%D0%B8%20%D0%BF%D0%BE%20MPI.pdf&name=%D0%92%D0%B2%D0%BE%D0%B4%D0%BD%D1%8B%D0%B5%20%D0%B7%D0%B0%D0%B4%D0%B0%D1%87%D0%B8%20%D0%BF%D0%BE%20MPI.pdf)
* [Папка с решениями](/intro_MPI/)
* [Скриншоты запуска](/intro_MPI/README.md)

# Вводные задачи по потокам
* [Условия задач](https://docs.yandex.ru/docs/view?url=ya-disk-public%3A%2F%2FWWP6MJYY23lknWYddiCjug8F2Ir9qyNSu57Ba5zP2OUR%2F%2FW55cFQjKU7UcdCNOWSRmR%2F0fePyGwwW%2FWKW0%2FCEA%3D%3D%3A%2F%D0%92%D0%B2%D0%BE%D0%B4%D0%BD%D1%8B%D0%B5%20%D0%B7%D0%B0%D0%B4%D0%B0%D1%87%D0%B8%20%D0%BF%D0%BE%20%D0%BF%D0%BE%D1%82%D0%BE%D0%BA%D0%B0%D0%BC.pdf&name=%D0%92%D0%B2%D0%BE%D0%B4%D0%BD%D1%8B%D0%B5%20%D0%B7%D0%B0%D0%B4%D0%B0%D1%87%D0%B8%20%D0%BF%D0%BE%20%D0%BF%D0%BE%D1%82%D0%BE%D0%BA%D0%B0%D0%BC.pdf)
* [Папка с решениями](/inro_threads/)
* [Скриншоты запуска](/inro_threads/README.md)

# Лабораторная работа №1
* [Условие](https://docs.yandex.ru/docs/view?url=ya-disk-public%3A%2F%2FWWP6MJYY23lknWYddiCjug8F2Ir9qyNSu57Ba5zP2OUR%2F%2FW55cFQjKU7UcdCNOWSRmR%2F0fePyGwwW%2FWKW0%2FCEA%3D%3D%3A%2F%D0%9B%D0%B0%D0%B1%D0%BE%D1%80%D0%B0%D1%82%D0%BE%D1%80%D0%BD%D0%B0%D1%8F_%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D0%B0_1.pdf&name=%D0%9B%D0%B0%D0%B1%D0%BE%D1%80%D0%B0%D1%82%D0%BE%D1%80%D0%BD%D0%B0%D1%8F_%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D0%B0_1.pdf)
* [Папка с выполнением](/1_lab/)

# Лабораторная работа №2
* [Условие](https://docs.yandex.ru/docs/view?url=ya-disk-public%3A%2F%2FWWP6MJYY23lknWYddiCjug8F2Ir9qyNSu57Ba5zP2OUR%2F%2FW55cFQjKU7UcdCNOWSRmR%2F0fePyGwwW%2FWKW0%2FCEA%3D%3D%3A%2F%D0%97%D0%B0%D0%B4%D0%B0%D1%87%D0%B0_%D0%B8%D0%BD%D1%82%D0%B5%D0%B3%D1%80%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%B22023.pdf&name=%D0%97%D0%B0%D0%B4%D0%B0%D1%87%D0%B0_%D0%B8%D0%BD%D1%82%D0%B5%D0%B3%D1%80%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5_%D0%B22023.pdf)
* [Папка с выполнением](/2_lab/)

# Как запустить сие добро
* Установить mpitch на Ubuntu: 
```
sudo apt install mpich
```

* Переходим к нужному файлу и компилим
```
cd <путь>/<до>/<папки с файлами,>/<например,>/intro_MPI
mpicc <имя входного файла>.c -o <имя выходного файла>
```
* Запуск
```
mpirun -np <количество процессов> ./<имя выходного файла>
```