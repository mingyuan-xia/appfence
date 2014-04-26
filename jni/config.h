/*
 * config.h
 * This header contains constants that alter the behavior of appfence
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define SANDBOX_ENABLED 1

#define SANDBOX_PATH "/data/data /data/user/0 /sdcard /mnt/sdcard /storage/sdcard"
#define SECOND_DIR "/data /sdcard"

#define SANDBOX_STORAGE_PATH "/data/sandbox/"
#define SANDBOX_LINK "/data/s"

#define SANDBOX_CONTENT_PROVIDER "com.android.contacts"
#define SANDBOX_CONTENT_PROVIDER_FAKE_PREFIX "fak"

#endif /* CONFIG_H_ */
