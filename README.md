appfence
========
Appfence is an android *sandbox* that provides both traditional file/network isolation as well as Android-specific sandboxing. Appfence provides two major functionalities:
* redirect files that accessed by the sandboxed app. Thus the sandboxed app cannot access files belonging to the same app in normal mode.
* Android API sandboxing, which is under investigation

Note: Appfence operates on system call level (ptrace-based)

## Getting start

### Eclipse + CDT + Android NDK + Android SDK
The project is mainly using `Android NDK` toolchain (compilers, scripts, debuggers) for development. Also `Android SDK` is needed if the actual device is needed for development.

### Eclipse setup
* We need some environment variables in Eclipse for the NDK path. Here are the pics.
* First setup the Android NDK path:
![][ndk_path]
* Second setup the environment variables. Note that on Windows the build script is `ndkbuild.cmd`. On Linux it is `ndkbuild`.
![][eclipse_env]
* Now import the project to Eclipse, you should see include/ resolved.

[ndk_path]: doc/ndk_path.png "Configure NDK path in Eclipse"
[eclipse_env]: doc/eclipse_env.png "Configure Eclipse environment variables for the project"

### Building the project
* There is no auto build enabled. So once the code is changed, you need to go to Run->Build All.
* Make sure NDK build works fine and no errors pop up (in best case, no warnings too).

### Testing (Run)
* The testng can be done in an emulated device or an actual device. An external tool is provided to upload the executable and launch it. The launch configuration is `appfence cpp.launch`, which in turns calls the `run.bat` script.

### Debugging
* The GDB way will come soon

### Adding src files
* The `Android.mk` in the jni folder is the build script, which is straightforward. Everytime you add new source files, add it to this makefile.
* The `Makefile` at the project root folder is just for non-Eclipse developers.
* Note that you cannot change the `jni` folder name, which is identified by the Android build tool chain.
