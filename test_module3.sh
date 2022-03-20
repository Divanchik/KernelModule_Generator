echo "major number: {$1}"
sudo mknod /dev/one c $1 0
head --bytes=1991 /dev/one >> ex.txt
sudo rm /dev/one
sudo rmmod module3
dmesg
