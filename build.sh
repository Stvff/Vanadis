set -e

#gcc -lm -Wall -g3 -fsanitize=address vic.c -o vic
#gcc vic.c -lm -Wall -o vic
if [[ $1 == fast ]]; then
	echo "fast"
	gcc vic.c -lm -Wall -Os -o vic
elif [[ $1 == lib ]]; then
	gcc libvic.c -lm -Wall -Os -c -fPIC
	gcc libvic.o -shared -o libvic.so
	rm libvic.o
	#sudo cp libvic.so /lib64/
	gcc vic.c -lm -lvic -D libraryincluded -Wall -Os -o vic
else
	gcc -lm -Wall -g3 -fsanitize=address vic.c -o vic
fi

cp Vanadis.yaml ~/.config/micro/syntax/
./vic test.vas

#vic scripts/fibon.vas
#./scripts/rule110.vas
