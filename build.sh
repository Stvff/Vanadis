set -ex

gcc -lm -Wall -g3 -fsanitize=address vic.c -o vic
#gcc vic.c -lm -Wall -o vic
#gcc vic.c -lm -Wall -Os -o vic

cp Vanadis.yaml ~/.config/micro/syntax/
./vic test.vas

#vic scripts/fibon.vas
#./scripts/rule110.vas