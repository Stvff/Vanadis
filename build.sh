set -e

out=vic

#gcc -lm -Wall -g3 -fsanitize=address vic.c -o vic
#gcc vic.c -lm -Wall -o vic
if [[ $1 == fast ]]; then
	echo "fast"
	gcc vic.c -lm -Wall -Os -o $out
elif [[ $1 == lib ]]; then
	gcc libvic.c -lm -Wall -Os -c -fPIC
	gcc libvic.o -shared -o libvic.so
	rm libvic.o
	#sudo cp libvic.so /lib64/
	gcc vic.c -lm -lvic -D libraryincluded -Wall -Os -o $out
else
	gcc -lm -Wall -g3 -fsanitize=address vic.c -o $out
fi

cp Vanadis.yaml ~/.config/micro/syntax/
./$out test.vas

#vic scripts/fibon.vas
#./scripts/rule110.vas
