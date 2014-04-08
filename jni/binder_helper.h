/*
 * binder_helper.h
 * function provide binder intercept
 */

#ifndef BINDER_HELPER_H_
#define BINDER_HELPER_H_

#define ISERVICE_MANAGER "android.os.IServiceManager"
#define ICONTENT_PROVIDER "android.content.IContentProvider"
#define ICONTENT_SERVICE "android.content.IContentService"
#define IACTIVITY_MANAGER "android.app.IActivityManager"
#define IPACKAGE_MANAGER "android.content.pm.IPackageManager"


// copy from adroid.os.IBinder
#define FIRST_CALL_TRANSACTION 0x00000001

// copy from include/binder/IServiceManager.h
enum{
	GET_SERVICE_TRANSACTION = FIRST_CALL_TRANSACTION,
	CHECK_SERVICE_TRANSACTION,
	ADD_SERVICE_TRANSACTION,
	LIST_SERVICES_TRANSACTION,
};

// copy from android.app.IActivityManager
enum{
	GET_CONTENT_PROVIDER_TRANSACTION = FIRST_CALL_TRANSACTION + 28,
};

// binder ioctl handler
void binder_ioctl_handler(pid_t pid);

#endif
