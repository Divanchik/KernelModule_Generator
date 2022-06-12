sudo rmmod mod_lfsra5
sudo dmesg -C
sudo insmod mod_lfsra5.ko
sudo mknod /dev/prng3 c 235 0

head -c 128 /dev/prng3 >> lfsra5.log
hexyl lfsra5.log
rm lfsra5.log

# sudo rm /dev/prng3
# sudo rmmod mod_lfsra5
# dmesg
