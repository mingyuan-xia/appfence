/*
 * config.h
 * This header contains constants that alter the behavior of appfence
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define SANDBOX_ARCH_ARM
// #define SANDBOX_ARCH_X86

#define SANDBOX_ENABLED 1

#define PROCESS_FILTER_ENABLED 0
#define PROCESS_FILTER_PATH "/data/local/filter.cfg"

#define FILE_SANDBOX_ENABLED 1
#define CONTENT_SANDBOX_ENABLED 0

#define SANDBOX_PATH_INTERNAL "/data/data /data/user/0"
#define SANDBOX_PATH_EXTERNAL "/mnt/sdcard /sdcard /storage/sdcard"

#define SANDBOX_PATH_INTERNAL_EXCLUDE "/lib"

#define SANDBOX_STORAGE_PATH "/data/sandbox/"
#define SANDBOX_LINK "/data/s"

#define SANDBOX_CONTENT_PROVIDER "com.android.contacts"
#define SANDBOX_CONTENT_PROVIDER_FAKE_PREFIX "com"

#endif /* CONFIG_H_ */

