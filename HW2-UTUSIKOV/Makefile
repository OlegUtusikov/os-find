all: build run

rebuild: clean build

build:
	mkdir build && cd build && cmake ../ && make

run:
	cd build && ./Find ./../test_dir -name file1.txt -exec /usr/bin/sha1sum
	cd build && ./Find ./../test_dir
	cd build && ./Find ./../test_dir -size -500 
	cd build && ./Find ./../test_dir -size +600 -exec /usr/bin/sha1sum
	cd build && ./Find ./../test_dir -size -100 -exec /usr/bin/sha1sum
	cd build && ./Find ./../test_dir -name file1.txt
	cd build && ./Find ./../test_dir -size =26
	cd build && ./Find ./../test_dir -name test1
	cd build && ./Find ./../test_dir -size -10 -inum 2246549 -exec /user/bin/sha1

clean:
	rm -rf build