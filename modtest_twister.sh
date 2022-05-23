sudo rmmod mod_twister
sudo dmesg -C
sudo insmod mod_twister.ko
sudo mknod /dev/prng2 c 235 0

head -c 128 /dev/prng2 >> twister.log
hexyl twister.log
rm twister.log

sudo rm /dev/prng2
sudo rmmod mod_twister
dmesg
