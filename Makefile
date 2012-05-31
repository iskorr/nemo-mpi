all:
	rm -rf build
	mkdir build
	cd build && cmake ../ -DCMAKE_CXX_COMPILER=/usr/bin/mpicxx -DNEMO_MATLAB_ENABLED=OFF
	cd build && make
	cd build && sudo make install

r:
	cd build && make

clean:
	rm -rf build
