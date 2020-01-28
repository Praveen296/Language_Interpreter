CRITICAL_SECTION	gLock;

struct tSyncFlag {

	int uFlag = 0;		// if 1 then readlock if 2 then write lock
	int uCount = 0;
};

class ReadLock {

	tSyncFlag* syncFlags;

public:
	ReadLock(tSyncFlag &pFlag) {

		syncFlags = &pFlag;

		EnterCriticalSection(&gLock);

		syncFlags->uFlag = 1;

		++syncFlags->uCount;

		LeaveCriticalSection(&gLock);
	}

	~ReadLock() {

		EnterCriticalSection(&gLock);

		--syncFlags->uCount;
		if (syncFlags->uCount == 0) {

			syncFlags->uFlag = 0;
		}

		LeaveCriticalSection(&gLock);
	}
};

class WriteLock {

	tSyncFlag* syncFlags;

public:
	WriteLock(tSyncFlag &pFlag) {

		syncFlags = &pFlag;

		EnterCriticalSection(&gLock);

		while (syncFlags->uFlag == 1) {

			LeaveCriticalSection(&gLock);
			Sleep(1);
			EnterCriticalSection(&gLock);
		}

		syncFlags->uFlag = 2;
		++syncFlags->uCount;
	}

	void WriteUnLock() {

		syncFlags->uFlag = 0;
		--syncFlags->uCount;

		LeaveCriticalSection(&gLock);
	}
};