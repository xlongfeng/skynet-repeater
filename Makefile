default_target: all

.PHONY : default_target

all: Debug/Makefile
	$(MAKE) -C Debug

.PHONY : all

Debug/Makefile: Makefile CMakeLists.txt 
	cmake -H. -BDebug -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=arm-atollic-eabi.cmake

clean:
	$(MAKE) -C Debug clean
	
.PHONY : clean