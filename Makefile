bin/argmax: src/argmax.cpp
	g++ src/argmax.cpp -g -std=c++11 -I /Users/djbutler/dev/halide/include -I /Users/djbutler/dev/halide/tools -L /Users/djbutler/dev/halide/bin -lHalide `libpng-config --cflags --ldflags` -o bin/argmax

bin/block_match_flow: src/block_match_flow.cpp
	g++ src/block_match_flow.cpp -g -std=c++11 -I /Users/djbutler/dev/halide/include -I /Users/djbutler/dev/halide/tools -L /Users/djbutler/dev/halide/bin -lHalide `libpng-config --cflags --ldflags` -o bin/block_match_flow

