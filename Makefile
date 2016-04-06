bin/argmax: src/argmax.cpp
	g++ src/argmax.cpp -g -std=c++11 -I /Users/djbutler/dev/halide/include -I /Users/djbutler/dev/halide/tools -L /Users/djbutler/dev/halide/bin -lHalide `libpng-config --cflags --ldflags` -o bin/argmax

bin/block_match_flow: src/block_match_flow.cpp
	g++ src/block_match_flow.cpp -g -std=c++11 -I /Users/djbutler/dev/halide/include -I /Users/djbutler/dev/halide/tools -L /Users/djbutler/dev/halide/bin -lHalide `libpng-config --cflags --ldflags` -o bin/block_match_flow

bin/block_match_flow_generate: src/block_match_flow_generate.cpp
	g++ src/block_match_flow_generate.cpp -g -std=c++11 -I /Users/djbutler/dev/halide/include -I /Users/djbutler/dev/halide/tools -L /Users/djbutler/dev/halide/bin -lHalide -o bin/block_match_flow_generate

include/block_match_flow_arm32.h: bin/block_match_flow_generate
	./bin/block_match_flow_generate
	mv block_match_flow_arm32.h include/
	mv block_match_flow_arm32.o lib/

include/block_match_flow_arm32.o: bin/block_match_flow_generate
	./bin/block_match_flow_generate
	mv block_match_flow_arm32.h include/
	mv block_match_flow_arm32.o lib/
