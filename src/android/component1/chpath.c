#include <unistd.h>
#include <string.h>
#include <stdio.h>

void createPath(char* dir)
{
	int	i,len;
	char str[512];
	strcpy(str, dir);
	len = strlen(str);
	for (i=0; i<len; i++)
	{
		if (str[i] == '/')
		{
			str[i] = '\0';
			if (access(str, F_OK) != 0)
			{
				mkdir(str, 0777);
			}
			str[i] = '/';
		}
	}
	return;
}

int changepath(char* str) 
{
	char tmp[strlen(str) + 1];
	char buffer[strlen(str) + 1];
	strcpy(tmp, str);
	strcpy(buffer, str);
	char* p = strtok(tmp, "/");
        if ((strcmp(p, "system") == 0) || (strcmp(p, "sys") == 0) || (strcmp(p, "dev") == 0) || (strcmp(p, "proc") == 0) || ((strcmp(p, "acct") == 0))) {
		return 0;
	}
	else if (strcmp(p, "data") == 0) {
		p = strtok(NULL, "/");
		if ((strcmp(p, "dalvik-cache") == 0) || (strcmp(p, "app") == 0)) {
			return 0;
		}
		char* filename = strrchr(buffer, '/');
		strcpy(str, "/data/sbx");
		strncat(str, filename, strlen(filename));
          /* str[strlen(str)] = 0; */
	}
	else if (strcmp(p, "sdcard") == 0) {
		char* filename = strrchr(buffer, '/');
		strcpy(str, "/sdcard/sbx");
		strncat(str, filename, strlen(filename));
	}
	else if (strcmp(p, "mnt") == 0) {
		p = strtok(NULL, "/");
		if ((strcmp(p, "sdcard") == 0)) {
			char* filename = strrchr(buffer, '/');
			strcpy(str, "/sdcard/sbx");
			strncat(str, filename, strlen(filename));
		}
	}
	/* else { */
	/* 	char* filename = strrchr(buffer, '/'); */
	/* 	strcpy(str, "/sdcard/sbx"); */
	/* 	strncat(str, filename, strlen(filename)); */
	/* } */
	createPath(str);
        return 1;
}
