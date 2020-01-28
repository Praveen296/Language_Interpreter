#include <conio.h>

using namespace std;

void CheckHandle(HANDLE pFile) {

	if (pFile == INVALID_HANDLE_VALUE) {

		cout << "File handle creation failed. Exiting" << GetLastError() << endl;
		Sleep(10000);
		exit(0);
	}
}

void Check(BOOL pVal) {

	if (!pVal) {

		cout << "I/O operation failed" << GetLastError() << endl;
		exit(0);
	}
}

void LeftTrim(char* pStr, int pInd) {

	int index = pInd;

	while (pStr[index] == ' '|| pStr[index] == '\t') {

		++index;
	}
	StrCpy(pStr, pStr + index);
}

void RightTrim(char* pStr) {

	int index = StrLen(pStr) - 1;

	while (pStr[index] == ' '|| pStr[index] == '\t') {

		--index;
	}
	pStr[++index] = '\0';
}

