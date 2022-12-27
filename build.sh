set -e

out=vic
CC=gcc

if [[ $1 == fast ]]; then
	echo "fast"
	$CC vic.c -lm -Wall -Os -o $out
elif [[ $1 == lib ]]; then
	$CC libvic.c -lm -Wall -Os -c -fPIC
	$CC libvic.o -shared -o libvic.so
	rm libvic.o
	#sudo cp libvic.so /lib64/
	$CC vic.c -lm -Wall -Os -lvic -D libraryincluded -o $out
else
	$CC -lm -Wall -g3 -fsanitize=address vic.c -o $out
fi

cp Vanadis.yaml ~/.config/micro/syntax/
./$out test.vas

#vic scripts/fibon.vas
#./scripts/rule110.vas
