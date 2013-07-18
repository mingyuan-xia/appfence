appfence
========
Appfence is an android *sandboxing* mechanism aims to prevent apps' misbehaviours from invoking SYSCALL and thus enables users to operate apps properly.

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
