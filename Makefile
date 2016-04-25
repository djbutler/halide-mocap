bin/argmax: src/argmax.cpp
	g++ src/argmax.cpp -g -std=c++11 -I /Users/djbutler/dev/halide/include -I /Users/djbutler/dev/halide/tools -L /Users/djbutler/dev/halide/bin -lHalide `libpng-config --cflags --ldflags` -o bin/argmax

bin/block_match_flow: src/block_match_flow.cpp
	g++ src/block_match_flow.cpp -g -std=c++11 -I /Users/djbutler/dev/halide-mocap/include -I /Users/djbutler/dev/halide/include -I /Users/djbutler/dev/halide/tools -L /Users/djbutler/dev/halide/bin -lHalide `libpng-config --cflags --ldflags` -o bin/block_match_flow

bin/block_match_flow_generate_arm32: src/block_match_flow_generate_arm32.cpp
	g++ src/block_match_flow_generate_arm32.cpp -g -std=c++11 -I /Users/djbutler/dev/halide/include -I /Users/djbutler/dev/halide/tools -L /Users/djbutler/dev/halide/bin -lHalide -o bin/block_match_flow_generate_arm32

include/block_match_flow_arm32.h: bin/block_match_flow_generate_arm32
	./bin/block_match_flow_generate_arm32
	mv block_match_flow_arm32.h include/
	mv block_match_flow_arm32.o lib/

lib/block_match_flow_arm32.o: bin/block_match_flow_generate_arm32
	./bin/block_match_flow_generate_arm32
	mv block_match_flow_arm32.h include/
	mv block_match_flow_arm32.o lib/

bin/block_match_flow_arm32: src/block_match_flow_arm32.cpp
	g++ src/block_match_flow_arm32.cpp -g -std=c++11 -I /home/ubuntu/dev/halide/include -I /home/ubuntu/dev/halide/tools -I /home/ubuntu/dev/halide-mocap/include -pthread -Wl,--no-as-needed -ldl lib/block_match_flow_arm32.o `libpng-config --cflags --ldflags` -o bin/block_match_flow_arm32
