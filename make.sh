set -e

out=vic
CC=gcc

if [[ $1 == help ]]; then
	echo "The Vanadis 'build' script"
	echo ""
	echo "./make.sh                    -- debug build"
	echo "./make.sh release            -- compiles with optimisations"
	echo "./make.sh lib                -- compiles with the dynamic library"
	echo "./make.sh windows            -- compiles for windows"
	echo "./make.sh profile            -- profiles the interpreter for some scripts"
	echo ""
	exit
elif [[ $1 == release ]]; then
	echo "release"
	$CC vic.c -lm -Wall -Os -o $out
elif [[ $1 == lib ]]; then
	echo "lib"
	$CC libvic.c -lm -Wall -Os -c -fPIC
	$CC libvic.o -shared -o libvic.so
	rm libvic.o
	#sudo cp libvic.so /lib64/
	$CC vic.c -lm -Wall -Os -lvic -D libraryincluded -o $out
elif [[ $1 == windows ]]; then
	echo "windows"
	out=$out.exe
	CC=x86_64-w64-mingw32-gcc
	$CC vic.c -lm -Wall -Os -o $out
elif [[ $1 == profile ]]; then
	echo "profile"
	$CC vic.c -lm -Wall -Os -o $out -pg
	./$out scripts/speed.vas
	mkdir -p ./profs/
	gprof $out gmon.out > profs/speed.prof
	echo "10000000\n\n" | $out scripts/fibon.vas
	gprof $out gmon.out > profs/fibon.prof
	rm gmon.out
else
	echo "debug"
	$CC -lm -Wall -g3 -fsanitize=address vic.c -o $out
fi

./$out -dc test.vas
cp Vanadis.yaml ~/.config/micro/syntax/
