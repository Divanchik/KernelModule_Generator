sudo rmmod mod_tauswort
sudo dmesg -C
sudo insmod mod_tauswort.ko
sudo mknod /dev/prng1 c 235 0

head -c 128 /dev/prng1 >> tauswort.log
hexyl tauswort.log
rm tauswort.log

sudo rm /dev/prng1
sudo rmmod mod_tauswort
dmesg
