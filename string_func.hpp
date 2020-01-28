#include <iostream>

using namespace std;

int StrLen(char *pStr) {

		int i = 0;
	while (pStr[i] != '\0')
		++i;
	return i;
}

int StrLen(const char *pStr) {

	int i = 0;
	while (pStr[i] != '\0')
		++i;
	return i;
}

void StrCpy(char* pStr1, char* pStr2) {
		int i;
	for (i = 0; i < StrLen(pStr2)+1; i++)
	{
		pStr1[i] = pStr2[i];
	}
	pStr1[i] = '\0';
	return;
}

bool StrCmp(char* pStr1, char* pStr2) {

	if (StrLen(pStr1) != StrLen(pStr2))
	{
		return 0;
	}
	for (int i = 0; i < StrLen(pStr1); i++)
	{

		if (pStr1[i] != pStr2[i])
			return 0;
	}
	return 1;
}

bool StrCmp(char* pStr1,const char* pStr2) {

	if (StrLen(pStr1) != StrLen(pStr2))
	{
		return 0;
	}
	for (int i = 0; i < StrLen(pStr1); i++)
	{

		if (pStr1[i] != pStr2[i])
			return 0;
	}
	return 1;
}

bool StrNCmp(char* pStr1,const char* pStr2, int pNum) {

	for (int i = 0; i < pNum; i++)
	{
		if (pStr1[i] == '\0' || pStr2[i] == '\0')
			return 0;
		if (pStr1[i] != pStr2[i])
		{
			return 0;
		}
	}
	return 1;
}

void StrCat(char* pStr1, const char* pStr2) {

	int n1 = StrLen(pStr1);
	int n2 = StrLen(pStr2);
	int i = 0;
	pStr1 = (char*)realloc(pStr1, (n1 + n2+1) * sizeof(char));
	while (pStr2[i] != '\0')
	{
		pStr1[i + n1] = pStr2[i];
		i++;
	}
	pStr1[i + n1] = '\0';

}

void StrReverse(char str[], int length)
{
	int start = 0;
	int end = length - 1;
	while (start < end)
	{
		swap(*(str + start), *(str + end));
		start++;
		end--;
	}
}

int CountDigit(long long n)
{
	int count = 0;
	while (n != 0) {
		n = n / 10;
		++count;
	}
	return count;
}

char* ItoA(int num,char* str)
{
	int base = 10;
	int i = 0;
	bool isNegative = false;

	if (num == 0) {

		str[i++] = '0';
		str[i] = '\0';
		return str;
	}
	if (num < 0 && base == 10) {

		isNegative = true;
		num = -num;
	}
	while (num != 0)
	{
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}
	if (isNegative)
		str[i++] = '-';

	str[i] = '\0'; 
	StrReverse(str, i);

	return str;
}