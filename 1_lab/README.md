# Лабораторная работа №1

## Задание к допуску
Скриншот запуска задания к допуску
<image src="into.png">

## Выполнение
Для выполнения выбрана разностная схема крест
Нужно найти численное решения для уравнение переноса:
```
∂u(t,x)/∂t + a*∂u(t,x)/∂x = f(t,x), 0≤t≤T, 0≤x≤X
u(0,x) = φ(x), 0≤x≤X
u(t,0) = ψ(t), 0≤t≤T
```
Для решения задачи используется равномерная сетка с шагами τ по времени и h по
координате. Функция u(t,x) рассматривается в точках t=kτ, x=mh, 0≤k≤K, 0≤m≤M, T=Kτ,
X=Mh.
Разностная схема крест записывается следующим образом:
```
(u[k+1][m] - u[k-1][m]) / (2*tau) + (u[k][m+1] - u[k][m-1]) / (2*h) = f[k][m], k = 0, ..., K - 1; m = 0, ..., M - 1
```
### Подробнее про разностную схему крест
Разностная схема "крест" (или "пятиточечная разностная схема") - это один из численных методов, используемых для аппроксимации производных в дифференциальных уравнениях. Она широко применяется в численных методах решения дифференциальных уравнений в частных производных. Для аппроксимации производных по координатам используется разностная схема "крест". При этом значения функции берутся в пяти точках: центральная точка и четыре соседние точки по горизонтали и вертикали.
```
    u[i, j+1]
        |
u[i-1, j] - u[i, j] - u[i+1, j]
        |
    u[i, j-1]
```
Подставляя эти аппроксимации в уравнение теплопроводности, мы получаем разностную схему, которая связывает значения функции в текущий момент времени и в следующий момент времени.

В некоторых случаях могут возникать проблемы с численной неустойчивостью, особенно при использовании больших шагов сетки или при аппроксимации нелинейных уравнений. Это может приводить к нефизичным или осциллирующим решениям (К чему создан соответствующий комментарий в коде)

### Подробнее про алгоритм
Сначала мы должны задать начальные условия, используя функции пси и фи. Далее мы используем метод прямоугольника для заполнения второго слоя так, как показано на схеме ниже:
```
 \0 0 0 0 0\   (1)   \+ 0 0 0 0\   (2)   \+ 0 0 0 0\
 \0 0 0 0 0\  ---->  \+ 0 0 0 0\  ---->  \+ 0 0 0 0\ 
 \0 0 0 0 0\         \+ 0 0 0 0\         \+ + + + +\
 \0 0 0 0 0\         \+ + + + +\         \+ + + + +\
```
Почему используем прямоугольник, - не хочется терять точность схемы крест.

Параллельный алгоритм состоит в том, чтобы разделить сетку на n одинаковых частей, где n - количество процессов. Проблема схемы крест состоит в том, что в границах мы не можем вычислить значения, так как схема выходит за границы сетки. Предлагаемое решение состоит в том, чтобы использовать блокирующую отправку Bsend, чтобы отправить на правую и левую границы значение процесса k. И последним шагом снова применяем прямоугольник.

### Что-то типа отчета о выполнении
Требовалось оценить теоретически и измерить ускорение и эффективность полученной реализации в зависимости от числа задействованных процессоров и размера задачи (шага сетки по времени).

Сначала представим один кадр из решений:
<image src="result.png">

Далее об ускорении и эффективности программы:
для нескольких прогонов с одинаковыми стартовыми данными были получены разные графики, например
<image src="statistic_x_10000.0_t_1.0.png">
и еще для тех же данных
<image src="1statistic_x_10000.0_t_1.0.png">

Аналогично для тех же данных, только срднее бралось по 20 запускам, а не по 10:
<image src="what.jpg">

Видно из графиков, что алгоритм эффективен при загрузке до 4 ядер, далее идет явная регрессия, хоть и общее поведение до конца не определено

## Контрольные вопросы
1. Ускорение и эффективность параллельных алгоритмов: Ускорение параллельного алгоритма определяет, во сколько раз он быстрее своего последовательного варианта при использовании определенного числа процессоров или ядер. Эффективность показывает, насколько эффективно используются доступные вычислительные ресурсы при выполнении параллельного алгоритма.

2. Закон Амдаля: Закон Амдаля определяет ограничение ускорения параллельной системы из-за невозможности полностью распараллелить всю программу. Он говорит о том, что ускорение ограничено долей последовательной части программы. Чем больше последовательная часть, тем меньше ускорение можно получить от параллельного выполнения.

3. Свойства канала передачи данных. Латентность: Канал передачи данных имеет несколько свойств, включая пропускную способность (скорость передачи данных), задержку (время, требуемое для передачи данных) и латентность (задержка, вызванная буферизацией данных в канале). Латентность влияет на время, которое требуется для доставки данных от отправителя к получателю и может быть критичной в системах реального времени.

4. Виды обменов "точка-точка": синхронные, асинхронные. Буферизация данных: Синхронные обмены точка-точка требуют согласованного времени отправки и получения данных между отправителем и получателем. Асинхронные обмены позволяют отправителю и получателю работать независимо друг от друга. Буферизация данных позволяет временно хранить данные перед отправкой или после получения, обеспечивая более эффективную коммуникацию в системах с различной скоростью отправки и получения данных.

5. Балансировка загрузки: статическая и динамическая: Статическая балансировка загрузки происходит на этапе проектирования системы и предполагает равномерное распределение задач между вычислительными ресурсами. Динамическая балансировка загрузки происходит во время выполнения и позволяет перераспределить задачи в зависимости от текущей загрузки ресурсов. Динамическая балансировка загрузки позволяет адаптировать распределение задач в реальном времени в зависимости от изменяющейся нагрузки и доступности вычислительных ресурсов. Она может быть основана на различных стратегиях, таких как самонастраиваемые алгоритмы, анализ нагрузки и механизмы миграции задач.

6. Геометрический параллелизм: Геометрический параллелизм связан с разделением задач по пространственным координатам. Это означает, что данные или задачи разделяются на подзадачи, которые могут быть выполнены независимо друг от друга. Этот тип параллелизма часто используется в задачах обработки изображений, симуляции физических систем и других задачах, где данные могут быть разделены на участки, которые обрабатываются параллельно.

7. Конвейерный параллелизм: Конвейерный параллелизм представляет собой разделение вычислительной задачи на последовательность подзадач, которые выполняются параллельно, но последовательно передают свои результаты следующей подзадаче. Такой подход позволяет увеличить пропускную способность и ускорить выполнение задачи. Конвейерный параллелизм широко применяется в задачах потоковой обработки данных, где различные этапы обработки могут выполняться независимо, но последовательно.