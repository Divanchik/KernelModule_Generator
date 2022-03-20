obj-m += module3.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
test1:
	sudo dmesg -C
	sudo insmod module1.ko
	sudo rmmod module1.ko
	dmesg
test2:
	# Мы ставим - перед командой rmmod чтобы игнорировать ошибки в случае, если модуль не загружен.
	-sudo rmmod module2
	# Очищаем лог ядра без echo
	sudo dmesg -C
	# Загружаем модуль
	sudo insmod module2.ko
	# Выводим лог ядра
	dmesg
test3:
	-sudo rmmod module3
	sudo dmesg -C
	sudo insmod module3.ko
	dmesg
test4:
	-sudo rmmod module4
	sudo dmesg -C
	sudo insmod module4.ko
	dmesg
