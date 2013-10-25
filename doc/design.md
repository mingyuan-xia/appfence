Design document
===============

Appfence has two components currently. The sandboxing component uses ptrace to sandbox the target Android app (given its pid and proper permission). The app launch intercepter cataches the target app process on creation.

## App launch intercepter
The most common way to start a new app is to click on the target app icon in the `Launcher` app. The following steps will be performed before a real process is "forked" and "exec-ed":

1. The `Launcher` app broadcast a "startActivity" intent, which is an async IPC.
2. The `SystemService` process receives and handles this intent.
3. `SystemService` checks if the app is already running. If so, the target app will be brought to foreground and no process will be created.
4. If the app has not yet started, `SystemService` will notify `zygote` to create the app process.
5. As the process manager, `zygote` will do the real `fork`, load the java binary and starts the activity.
Note that `zygote` runs a warm Dalvik VM so `exec` is not required.

So in the context of the launch initiator (the `Launcher` app, no `fork` will be observed). To allow the sandbox to ptrace the target app on creation, Appfence needs to take control at the moment that the target app process is forked. Below shows how we hijack the `zygote` process to achieve this:

![][initial]

1. Appfence first ptrace-attaches `zygote` to monitor all its syscalls. Note that appfence will attach with the `PTRACE_O_TRACEFORK` option such that any child process forked by `zygote` will be automatically ptraced.
2. Appfence broadcast the intent to start the new app and waits until `zygote` forks the target. 
3. Once `zygote` calls `fork()`, appfence will pause `zygote` on `fork()` return and pause the target app on start (thanks to `PTRACE_O_TRACEFORK`).
4. Appfence now knows the pid of the target app and hands it over to the sandboxing component. Now it is time to let go `zygote`.

## Sandboxing

## Reference 
* [StackOverflow: How to ptrace a multi-threaded application?](http://stackoverflow.com/questions/5477976/how-to-ptrace-a-multi-threaded-application)
* [Ptrace-Linux man page](http://linux.die.net/man/2/ptrace)
[initial]: initial.jpg "Initial phase"
