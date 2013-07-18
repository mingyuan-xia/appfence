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

	$./atrace <pid>
