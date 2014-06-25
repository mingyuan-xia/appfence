/*
 * util.c
 * Implementation of util.h
 */
#include <stdio.h>
#include <string.h>
#include <fnmatch.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>
#include "util.h"

/*
 * if it acquire a class name, return non-zero.
 */
bool_t get_app_process_class_name(pid_t pid, char *class_name_buf,
		int class_name_buf_size) {
	char path[256];
	char buf[1024] = { '\0' };
	int len;
	FILE *fin;
	char *s;
	char *c;
	char *class_name = NULL;

	/* verify the process is android app_process */
	snprintf(path, sizeof(path), "/proc/%d/exe", pid);
	len = readlink(path, buf, sizeof(buf) - 1);
	if (len <= 0) {
		printf("process info is null:%s\n", path);
		return false;
	}
	buf[len] = '\0';
	if (fnmatch(buf, "/system/bin/app_process", FNM_PATHNAME) != 0) {
		printf("process info is mismatch:%s\n", buf);
		return false;
	}

	snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
	fin = fopen(path, "r");
	if (fin == NULL) {
		printf("read cmdline fail:%s\n", path);
		return false;
	}
	buf[0] = '\0';
	fgets(buf, sizeof(buf), fin);
	fclose(fin);

	s = buf;
	while (s != NULL && *s != '\0') {
		while (*s == '\x20') {
			s++;
		}
		if (*s == '\0') {
			break;
		}
		c = strchr(s, '\x20');
		if (c)
			*c = '\0';
		if (strcmp(s, "--zygote") == 0) {
		} else if (strcmp(s, "--start-system-server") == 0) {
		} else if (strcmp(s, "--zygote") == 0) {
		} else if (strcmp(s, "--application") == 0) {
		} else if (strncmp(s, "--nice-name=", 12) == 0) {
		} else {
			class_name = s;
		}
		if (c != NULL) {
			s = c + 1;
		} else {
			s = NULL;
		}
	}

	strncpy(class_name_buf, class_name, class_name_buf_size);
	class_name_buf[class_name_buf_size - 1] = '\0';
	return true;
}

pid_t find_app_process(const char *class_name) {
	char buf[1024];
	char path_buf[256];
	struct dirent *dp;
	DIR *dir;
	pid_t app_pid = 0;

	dir = opendir("/proc");
	if (!dir)
		return 0;

	while ((dp = readdir(dir)) != NULL) {
		pid_t pid;
		int len;
		if (dp->d_type != DT_DIR)
			continue;

		const char *d = dp->d_name;
		while (*d != '\0' && isdigit(*d)) {
			d++;
		}
		if (*d != '\0')
			continue;

		snprintf(path_buf, sizeof(path_buf), "/proc/%s/exe", dp->d_name);
		len = readlink(path_buf, buf, sizeof(buf) - 1);
		if (len <= 0) {
			// printf("process info is null:%s\n", buf2);
			continue;
		}

		buf[len] = 0;
		if (fnmatch(buf, "/system/bin/app_process", FNM_PATHNAME) != 0) {
			// printf("process info is mismatch:%s\n", buf);
			continue;
		}

		pid = atoi(dp->d_name);
		if (get_app_process_class_name(pid, buf, sizeof(buf))) {
			if (strcmp(class_name, buf) == 0) {
				app_pid = pid;
				break;
			}
		}
	}
	closedir(dir);
	return app_pid;
}

void kill_app_process(bool_t force, const char *class_name) {
	pid_t pid = find_app_process(class_name);
	if (pid != 0) {
		kill(pid, SIGTERM);
	}
	printf("kill_app_process:%d, %s, len=%d\n", pid, class_name, strlen(class_name));
}

void kill_all_process_in_whitelist(const char *list_path) {
	FILE *fin = fopen(list_path, "rt");
	if (fin == NULL)
		return;

	int ret = 0;
	char buff[1024];
	char *s;
	while ((s = fgets(buff, sizeof(buff) - 1, fin)) != NULL) {
		int i = strlen(s) - 1;
		while (i >= 0 && (s[i] == '\x0d' || s[i] == '\x0a')) {
			s[i] = '\0';
			i--;
		}
		kill_app_process(true, s);
	}
	fclose(fin);
}
