all:
	rm -rf build
	mkdir build
	cd build && cmake ../ -DCMAKE_CXX_COMPILER=/usr/bin/mpicxx
	cd build && make
	cd build && make install

r:
	cd build && make

clean:
	rm -rf build
