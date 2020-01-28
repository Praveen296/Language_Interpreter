class TimeStamps {

public:

	SYSTEMTIME	uTime;
	bool		uStatus;
	char*		uCmd;
	char*		uName;
	TimeStamps* uNext;
	//int			period;

	TimeStamps() {

		uCmd = NULL;
		uStatus = 0;
		GetLocalTime(&uTime);
		uNext = NULL;
		uName = NULL;
		//period = 0;
	}
};