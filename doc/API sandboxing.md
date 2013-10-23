Methods for API sandboxing
==========================
# Introduction
* Android APIs are all fulfilled via binder IPCs. An API call is a binder transaction. The IPC number, the parameters are marshaled into a buffer and send to the binder kernel driver.
* By stracing the app, we know that a single API call involves two `ioctl` syscalls.
* Theoretically, intercepting these two `ioctl` allows us to decode the binder transaction, but this involves lots of engineering efforts.
* In this doc, we discuss several alternatives to sandboxing Android APIs, especially their pros and cons.

## Fake system server
* We create a fake system server, by cloning the code base from the original Android repository.
* Then we implement two important building blocks: duplicates a binder transaction and redirect one.
* Essentially, all APIs from the target app is first dispatch to our faking syserver. Our syserver can choose to redirect it to the real syserver or issue a new transaction to the real syserver and retrieve its results.
* The difficulties here is that all the supported IPCs in the syserver should be properly rewritten (either redirect or duplicate), which involves somehow large efforts.
* This might also incur notable overhead since one APIs now need two binder transactions.

## Instrumentating the system server
* In this method, we inject our code into the original system server, hooking the handler for the API of interest.
* This involves a stable code injection method that can tame DalvikVM.

## Ptracing the system server
* ptrace the handler for APIs initiated from the target apps.
* ptrace is syscall level, when the syserver is paused. It is paused at some JIT-ed code in the dalvik VM. It is not easy to inject the target app on this pause.
* ptrace may requires to stop on every syscalls involved, which can be slow.
