set -ex
gcc libvic.c -lm -Wall -Os -c -fPIC
gcc libvic.o -shared -o libvic.so
rm libvic.o
#sudo cp libvic.so /lib64/
gcc vic.c -lm -lvic -D libraryincluded -Wall -Os -o vic
./vic test.vic
