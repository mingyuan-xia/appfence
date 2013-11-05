Getting start
=============

= Prerequistie
The project is using Android NDK toolchain (compilers, scripts, debuggers). We need `Android SDK` and `Android NDK`.

= Eclipse setup
* We need some environment variables in Eclipse for the NDK path. Here are the pics.
* First setup the Android NDK path:
![][ndk_path]
* Second setup the environment variables
![][eclipse_env]
* Now import the project to Eclipse, you should see include/ resolved.

= Building the project
* There is no auto build enabled. So once the code is changed, you need to go to Run->Build All.
* Make sure NDK build works fine and no errors pop up

= Testing
* The GDB way will come soon
* Right now, use `adb push` to push the file at `<project_dir>/libs/armeabi/appfence` to the device for your test


[ndk_path]: ndk_path.png "Configure NDK path in Eclipse"
[eclipse_env]: eclipse_env.png "Configure Eclipse environment variables for the project"

