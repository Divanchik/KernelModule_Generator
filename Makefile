obj-m += mod_tauswort.o
obj-m += mod_twister.o
obj-m += mod_lfsra5.o
obj-m += mod_a5uni.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules CFLAGS_MODULE+="-msse2 -fno-common"
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

