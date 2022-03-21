echo "Major number: $1"
sudo mknod /dev/one c $1 0
if [ -e ex.txt ]
then
rm ex.txt
fi
head --bytes=1991 /dev/one >> ex.txt
sudo rm /dev/one
sudo rmmod module3
dmesg
