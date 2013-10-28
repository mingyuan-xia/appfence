# dummy Makefile
# You should install android NDK and include in the PATH

export PATH := $(PATH):/usr/local/android-ndk-r9

all:
	ndk-build -B

clean:
	ndk-build clean
