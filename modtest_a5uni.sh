sudo rmmod mod_a5uni
sudo dmesg -C
sudo insmod mod_a5uni.ko
sudo mknod /dev/prng4 c 235 0

head -c 128 /dev/prng4 >> a5uni.log
hexyl a5uni.log
rm a5uni.log

sudo rm /dev/prng4
sudo rmmod mod_a5uni
dmesg
