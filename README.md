appfence
========
Appfence is an android *sandboxing* mechanism providing a list of important features:
* intercept sensitive behaviors of apps and report them to user for further operation
* achieve file-level data isolation

Note: Appfence operates on system call level.

getting started
===============
We've run ptrace on android emulator.

First, set Android NDK cross-compiler environment.

	$cd src
	$make

Then use adb to push atrace into android emulator.
Use adb shell to run atrace like:
         
        $ cd .../tools
        $ ./emulator -avd your_avd_name      :start an android emulator
        
        $ cd .../platform-tools
        $ ./adb push atrace /data/atrace     :push atrace to emulator
        $ ./adb shell 
        $ ps                                 :to check pid for each running process
        $ cd data                           
        $ ./atrace <pid>                     :start atrace to monitor SYSCALLS
