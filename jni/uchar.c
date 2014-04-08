#include "uchar.h"

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
