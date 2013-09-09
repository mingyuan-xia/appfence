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

char* changepath(char* str) 
{
	char tmp[strlen(str)];
	strcpy(tmp, str);
	char* p = strtok(tmp, "/");
	if ((strcmp(p, "system") == 0) || (strcmp(p, "sys") == 0) || (strcmp(p, "dev") == 0) || (strcmp(p, "proc") == 0)) {
		return str;
	}
	if (strcmp(p, "data") == 0) {
		p = strtok(NULL, "/");
		if (strcmp(p, "dalvik-cache") == 0) {
			return str;
		}
	}
	char newPath[128] = "/sdcard/sandbox";
	strcat(newPath, str);
	str = newPath;
	createPath(str);
	return str;
}
