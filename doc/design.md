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

### File sandboxing
The goal of file sandboxing is to isolate the files accessed from the sandboxed app and the files from the same app in its usual mode. To achieve this, we use ptrace to intercept the `open()` system call of the target application and replace the file path being accessed. Here is the prototype for the `open()` system call. It takes a file path and a flag and returns a file descriptor:
<pre>
int open (const char *path, int oflag);
</pre>
Thus to sandbox the file, we need to replace this path such that the application thinks it is accessing the original file while it is accessing a file in our sandbox folder. To achieve this, we need to change the path. Assume the requested file was `/data/target_app/1.txt`. The most straightforward way is to add a prefix, whereas the new path becomes `/sandbox/data/target_app/1.txt`. Unfortunately, the buffer for `path` may not be able to accommodate the entire prefixed path string. So we need workarounds:

1. The cleanest way is to allocate a thread-local memory in the target application and copy the new path in it. This involves memory allocation which can be complicated (need code injection, hooking and multithreading-safe).
2. If we want to avoid the memory allocation, the buffer for the original path should be used for the new path, too. However, the path buffer may not be large enough to hold the entire new path, in which case buffer overflow will happen. This might still work if we remember the data in the overflowed region before `open()` and write it back once `open()` is done. Neverthreless, overflow can fail if:
  * The overflowed region after the path buffer is not writable. Consider the case when the path buffer in on the boundary of a page. The overflowed region crosses a page to another non-writable/non-present/invalid page.
  * The overflowed region may be concurrently used by another thread. For example the path buffer is allocated on the heap and its adjacent memory object is allocated by another thread. In this case, buffer overflow will overwrite the memory object of another thread which can be using at the same time. In some sense, the false sharing here can lead to correctness problems.
3. To overcome the difficulty of allocating memory in the application and avoid the hazard of buffer overflow, we introduce the **path shortening** trick. For a given path `a` with length `l`, we produce a shortened path `b` such that the length of `b` is shorter than `l` while `b` points to a file in the sandbox folder. For example, `a=/data/data/1.txt` with a length of 17 (`\0` included) can be shortened to be `b=/s/1.txt`. As long as `/s/` folder is a symbolic link of the sandbox folder, the file being accessed reside in the sandbox.
4. To fully enable this trick, we need to concern some more cornor cases and transparency issues:
  * The original path is too short e.g. `a=/1`. Fortunately, in Android, all paths that need to be shortened contain either `/data` or `/sdcard` prefix. So we will have the space to shorten the path.
  * The path is a relative path. In this case, the application working directory needs to be redirected to the sandbox folder.
  * We also need to make sure that the target application cannot [obtain the shortened file path from the file descriptor](http://stackoverflow.com/questions/1188757/getting-filename-from-file-descriptor-in-c).
  * Sometimes, we need to recode the file name being accessed to shorten the path, e.g., `/d/a_very_long_name`->`/s/d/recoded`.

## Reference 
* [StackOverflow: How to ptrace a multi-threaded application?](http://stackoverflow.com/questions/5477976/how-to-ptrace-a-multi-threaded-application)
* [Ptrace-Linux man page](http://linux.die.net/man/2/ptrace)
[initial]: initial.jpg "Initial phase"
