Getting start
=============

## Prerequistie
The project is mainly using `Android NDK` toolchain (compilers, scripts, debuggers) for development. Also `Android SDK` is needed if the actual device is needed for development.

## Eclipse setup
* We need some environment variables in Eclipse for the NDK path. Here are the pics.
* First setup the Android NDK path:
![][ndk_path]
* Second setup the environment variables. Note that on Windows the build script is `ndkbuild.cmd`. On Linux it is `ndkbuild`.
![][eclipse_env]
* Now import the project to Eclipse, you should see include/ resolved.

[ndk_path]: ndk_path.png "Configure NDK path in Eclipse"
[eclipse_env]: eclipse_env.png "Configure Eclipse environment variables for the project"

## Building the project
* There is no auto build enabled. So once the code is changed, you need to go to Run->Build All.
* Make sure NDK build works fine and no errors pop up (in best case, no warnings too).

## Testing
* The GDB way will come soon
* Right now, use `adb push` to push the file at `<project_dir>/libs/armeabi/appfence` to the device for your test

## Adding src files
* The `Android.mk` in the jni folder is the build script, which is straightforward. Everytime you add new source files, add it to this makefile.
* The `Makefile` at the project root folder is just for non-Eclipse developers.
* Note that you cannot change the `jni` folder name, which is identified by the Android build tool chain.
