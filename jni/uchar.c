#include "uchar.h"
#include <stdio.h>
#include <string.h>

int strcmp12(char16_t* str1, char* str2)
{
	int i;
	for(i = 0; str1[i] != 0 && str2[i] != 0; i++) {
		if(str1[i] > (unsigned short)str2[i]) {
			return 1;
		} else if (str1[i] < (unsigned short)str2[i]) {
			return -1;
		}
	}
	if(str1[i] == 0 && str2[i] == 0) {
		return 0;
	} else if(str1[i] != 0) {
		return 1;
	} else {
		return -1;
	}
}

int strcmp16(char16_t* str1, char16_t* str2)
{
	int i;
	for(i = 0; str1[i] != 0 && str2[i] != 0; i++) {
		if(str1[i] > str2[i]) {
			return 1;
		} else if (str1[i] < str2[i]) {
			return -1;
		}
	}
	if(str1[i] == 0 && str2[i] == 0) {
		return 0;
	} else if(str1[i] != 0) {
		return 1;
	} else {
		return -1;
	}

}

int strpre12(char16_t* str1, char* str2)
{
	int i;
	for(i = 0; str1[i] != 0 && str2[i] != 0; i++) {
		if(str1[i] != (unsigned short)str2[i]) {
			return -1;
		}
	}
	if(str1[i] == 0 && str2[i] != 0) {
		return -1;
	}
	return 0;
}

int strpreg12(char16_t* str1, char* strg2)
{
	int len = strlen(strg2);
	char tmp[len + 1];
	strcpy(tmp, strg2);
	char* current = tmp;
	while(current < tmp + len + 1) {
		char* last = current;
		for(;*last != 0 && *last != ' '; last++) {}
		*last = 0;
		if(strpre12(str1, current) == 0) {
			return 0;
		}
		current = last + 1;
	}
	return -1;
}

void strchpre12(char16_t* str1, char* str2)
{
	int i;
	for(i = 0; str1[i] != 0 && str2[i] != 0; i++) {
		str1[i] = (unsigned short) str2[i];
	}
}
