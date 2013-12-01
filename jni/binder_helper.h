/*
 * binder_helper.h
 * function provide binder intercept
 */

#ifndef BINDER_HELPER_H_
#define BINDER_HELPER_H_

#define ISERVICE_MANAGER "android.os.IServiceManager"
typedef short char16_t;

// copy from include/binder/IServiceManager.h
enum{
	GET_SERVICE_TRANSACTION = 0x00000001,
	CHECK_SERVICE_TRANSACTION,
	ADD_SERVICE_TRANSACTION,
	LIST_SERVICES_TRANSACTION,
};


// binder ioctl handler
void binder_ioctl_handler(pid_t pid);

#endif
