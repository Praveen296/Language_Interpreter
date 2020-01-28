#include "headers.hpp"
#include "globals.hpp"

#define BUFFSIZE 100
#define TBUFFSIZE 10

bool Check(HANDLE pHandle) {

	if (!pHandle) {

		cout << "Thread creation failed" << endl;
		return 0;
	}
	return 1;
}


DWORD WINAPI CatchEscape(void* pData) {

		char ch;
	do {

		ch = _getch();
	} while (ch != 27);

	gEscape = 1;
	return 0;
}

void DisplayStatus() {

		tSyncFlag	flags;
		TimeStamps* temp	= gStart;
		int			i		= 1;
		HANDLE		thread	= CreateThread(NULL, 0, CatchEscape, NULL, 0, NULL);

	while (!gEscape) {

		ReadLock r(flags);
		if (temp->uNext != NULL) {

			temp = temp->uNext;
			cout << endl << " " << i << ")\t" << temp->uTime.wDay << "-" << temp->uTime.wMonth << "-" << temp->uTime.wYear << " " << temp->uTime.wHour << ":" << temp->uTime.wMinute << ":" << temp->uTime.wSecond << "\t" << (temp->uTime.wHour < 12? "AM" : "PM") << "\t" << temp->uName << "\t" << temp->uCmd << "\t - \t" << (temp->uStatus ? "FAILED" : "SUCCESS");
			++i;
		}
		else {

			continue;
		}
	}
	CloseHandle(thread);
	return;
}

void DisplayTasks(int pIndex, TaskData* pTask) {

		int			i		= 0;
		HANDLE		thread	= CreateThread(NULL, 0, CatchEscape, NULL, 0, NULL);

	while (!gEscape) {

		if (i != pIndex) {

			cout << endl << " " << i + 1 << ")\t" << pTask[i].uName << "\t" << pTask[i].uCmd << "\t - \t" << pTask[i].uPeriod;
			++i;
		}
	}
	CloseHandle(thread);
	return;
}

DWORD WINAPI TaskExec(void* pData) {

		tSyncFlag		flags;
		bool			check;
		TaskData*		task_data	= (TaskData*)pData;
		const char*		command		= " 2> nul";
		int				len			= StrLen(task_data->uCmd) + 9;
		char*			cmd			= new char[len];

	strcpy(cmd, task_data->uCmd);
	strcat(cmd, command);

	while (!gFlag) {

		TimeStamps* Node = new TimeStamps();
		check = system(cmd);
		WriteLock w(flags);
		gCurrent->uNext = Node;
		gCurrent = gCurrent->uNext;
		gCurrent->uStatus = check;
		gCurrent->uCmd = task_data->uCmd;
		gCurrent->uName = task_data->uName;
		w.WriteUnLock();
		Sleep(task_data->uPeriod * 1000);
	}

	return 0;
}

bool AddTask(TaskData &pTask) {

		HANDLE	thread;

	thread = CreateThread(NULL, 0, TaskExec, (void*)&pTask, 0, NULL);
	if (!Check(thread)) {

		return 0;
	}

	return 1;

}

int ReadLine(HANDLE pFile, char* pBuff) {

		BOOL    return_value;
		DWORD	bytesread		= NULL;
		char    ch[1]			= { '0' };
		int		i				= 0;

	while (ch[0] != '\n') {

		return_value = ReadFile(pFile, ch, 1, &bytesread, NULL);
		Check(return_value);
		pBuff[i] = ch[0];
		++i;
		if (i % 100 == 0) {

			pBuff = (char*)realloc(pBuff, (((i - 1) / (BUFFSIZE - 1)) + 1) * BUFFSIZE * sizeof(char));
		}
	}
	i = i - 2;
	pBuff[i] = '\0';
	return i + 2;
}

int CheckValidOp(char* pBuff) {

		int		flag	= 0; //if 0 - not valid. 1 - TASK. 2 - SORT.
		int		i		= 0;

	LeftTrim(pBuff, i);
	if (pBuff[i] == '[') {

		LeftTrim(pBuff, i + 1);
		for (int j = 0; j < gOp.uNum_of_ops; ++j) {

			if (StrNCmp(pBuff, gOp.uOp[j], StrLen(gOp.uOp[j]))) {

				LeftTrim(pBuff, i + StrLen(gOp.uOp[j]));
				if (pBuff[i] == ']') {

					RightTrim(pBuff);
					if (pBuff[++i] == '\0') {
						flag = j + 1;
						break;
					}
				}
			}
		}
	}
	return flag;
}

int StrCmp(TaskData pTask1, TaskData pTask2, int pInd, tConfigData* pTaskList) {

		int i;
	for (i = 0; i < gFld.uNum_of_fields; ++i) {

		if (StrCmp(pTaskList->uTask_data->uSort[pInd].uOn, gFld.uFld[i])) {

			break;
		}
	}
	switch (i)
	{
	case 0:
		return strcmp(pTask1.uName, pTask2.uName);
	case 1:
		return strcmp(pTask1.uCmd, pTask2.uCmd);
	case 2:
		return(pTask1.uPeriod > pTask2.uPeriod);
	}
}

void Sort(tConfigData* &pTaskList) {

		TaskData temp;
	for (int i = 0; i < pTaskList->uTask_data->uIndex; i++)
	{
		for (int j = i + 1; j < pTaskList->uTask_data->uIndex; j++)
		{
			if (StrCmp(pTaskList->uTask_data->uTask[j], pTaskList->uTask_data->uTask[i], pTaskList->uInd, pTaskList) > 0) {

				temp = pTaskList->uTask_data->uTask[i];
				pTaskList->uTask_data->uTask[i] = pTaskList->uTask_data->uTask[j];
				pTaskList->uTask_data->uTask[j] = temp;
			}
		}
	}
}

void SetField(int pOpt, char* pField, TaskData* &pTask, int pIndex) {

	if (pOpt == 0) {

		return;
	}
	else if (pOpt == 2) {

		pTask[pIndex].uCmd = new char[strlen(pField) + 1];
		pTask[pIndex].uCmd[strlen(pField)] = '\0';
		strcpy(pTask[pIndex].uCmd, pField);
	}

	else if (pOpt == 3) {

		pTask[pIndex].uPeriod = atoi(pField);
	}

	else if (pOpt == 1) {

		pTask[pIndex].uName = new char[strlen(pField) + 1];
		pTask[pIndex].uName[strlen(pField)] = '\0';
		strcpy(pTask[pIndex].uName, pField);
	}

	return;
}

void SetSortBlock(int pOpt, char* pOn, SortData* &pSort_data, int pIndex) {

		int		i				= 0;
		bool	is_duplicate	= 0;
	if (pOpt == 0) {

		return;
	}
	else if (pOpt == 1) {

		while (i != pIndex) {

			if (StrCmp(pSort_data[i].uOn, pOn)) {

				is_duplicate = 1;
				break;
			}
			++i;
		}
		if (!is_duplicate) {

			pSort_data[pIndex].uOn = new char[strlen(pOn) + 1];
			pSort_data[pIndex].uOn[strlen(pOn)] = '\0';
			strcpy(pSort_data[pIndex].uOn, pOn);
		}
	}
	else {


		pSort_data[pIndex].uStr = new char[strlen(pOn) + 1];
		pSort_data[pIndex].uStr[strlen(pOn)] = '\0';
		strcpy(pSort_data[pIndex].uStr, pOn);
	}
}

int CheckValidSort(char* pBuff, SortData* pSort_data, int pInd) {

		int		flag = 0;
		int		i	 = 0;

	LeftTrim(pBuff, i);
	for (int j = 0; j < gSrt.uNum_of_fields; ++j) {

		if (StrNCmp(pBuff, gSrt.uFld[j], StrLen(gSrt.uFld[j]))) {

			LeftTrim(pBuff, i + StrLen(gSrt.uFld[j]));
			if (pBuff[i] == ':') {

				LeftTrim(pBuff, ++i);
				RightTrim(pBuff);
				if (j == 0) {

					for (int k = 0; k < gFld.uNum_of_fields; ++k) {

						if (StrNCmp(pBuff, gFld.uFld[k], StrLen(gFld.uFld[k]))) {

							flag = j + 1;
							break;
						}
					}
				}
				else { flag = j + 1; }
			}
		}
	}
	SetSortBlock(flag, pBuff, pSort_data, pInd);
	return flag;
}

int CheckValidField(char* pBuff, TaskData* &pTask, int pInd) {

		int   flag = 0; //if 0 - not valid. 1 - name. 2 - cmd. 3 - per.
		int	  i = 0;

	LeftTrim(pBuff, i);
	for (int j = 0; j < gFld.uNum_of_fields; ++j) {

		if (StrNCmp(pBuff, gFld.uFld[j], StrLen(gFld.uFld[j]))) {

			LeftTrim(pBuff, i + StrLen(gFld.uFld[j]));
			if (pBuff[i] == ':') {

				LeftTrim(pBuff, ++i);
				RightTrim(pBuff);
				flag = j + 1;
				break;
			}
		}
	}

	SetField(flag, pBuff, pTask, pInd);
	return flag;
}

void isTaskPresent(TaskData* &pTask, int &pInd) {

	if (pTask[pInd].uCmd != nullptr && pTask[pInd].uPeriod != 0) {

		if (pTask[pInd].uName == NULL) {

				const char* name	= { "unnamed" };
				int			len		= CountDigit(pInd + 1) + 1;
				char*		temp	= (char*)malloc(len);
			ItoA(pInd + 1, temp);
			strcat(temp, name);
			pTask[pInd].uName = new char[strlen(temp) + 1];
			strcpy(pTask[pInd].uName, temp);
		}
		AddTask(pTask[pInd]);
		++pInd;
		if (pInd % 10 == 0) {

			pTask = (TaskData*)realloc(pTask, (((pInd - 1) / (TBUFFSIZE - 1)) + 1) * TBUFFSIZE * sizeof(TaskData));
		}
	}
}

void IsSortPresent(SortData* &pSort_data, int &pIndex) {

	if (pSort_data[pIndex].uOn != NULL && pSort_data[pIndex].uStr != NULL) {

		++pIndex;
	}
}

int Parser(HANDLE pHandleFile, TaskData* &pTask, SortData* &pSort_data, int &pSort_ind) {

		DWORD	 file_size;
		int		 ret_val;						//returned value from CheckValidOp. 0 when it is not valid block (task/sort here). 
		int		 return_fld;					//returned value from CheckValidField. 0 when there is no valid attribute data in current line read.
		int	 	 op_repeat_check	= 0;		//to handle cases where [TASK]/[SORT] blocks comes without any attributes & data.
		int		 bytesread;
		char*	 buff				= (char*)malloc(BUFFSIZE);
		int		 ind				= 0;

	file_size = GetFileSize(pHandleFile, NULL);
	while (file_size != 0) {

		bytesread = ReadLine(pHandleFile, buff);
		ret_val   = CheckValidOp(buff);
		file_size = file_size - bytesread;

		do {

			if (ret_val == 1) {

				bytesread = ReadLine(pHandleFile, buff);
				file_size = file_size - bytesread;
				op_repeat_check = CheckValidOp(buff);

				if (op_repeat_check == 1) {

					op_repeat_check = 0;
					isTaskPresent(pTask, ind);
					IsSortPresent(pSort_data, pSort_ind);
					continue;
				}
				else if (op_repeat_check == 2) {

					op_repeat_check = 0;
					ret_val = 2;
					isTaskPresent(pTask, ind);
					IsSortPresent(pSort_data, pSort_ind);
					continue;
				}

				else { return_fld = CheckValidField(buff, pTask, ind); }
			}

			if (ret_val == 2) {

				bytesread = ReadLine(pHandleFile, buff);
				file_size = file_size - bytesread;
				op_repeat_check = CheckValidOp(buff);

				if (op_repeat_check == 1) {

					op_repeat_check = 0;
					ret_val = 1;
					isTaskPresent(pTask, ind);
					IsSortPresent(pSort_data, pSort_ind);
					continue;
				}
				else if (op_repeat_check == 2) {

					op_repeat_check = 0;
					isTaskPresent(pTask, ind);
					IsSortPresent(pSort_data, pSort_ind);
					continue;
				}

				else { 

					return_fld = CheckValidSort(buff, pSort_data, pSort_ind); 
				}
			}
		} while (return_fld && file_size != 0);

		isTaskPresent(pTask, ind);
		IsSortPresent(pSort_data, pSort_ind);
	}
	return ind;
}

void TaskStatus(void * pGlobalCtx, void * pLocalCtx) {

	system("CLS");
	cout << "\n			Task Status	   \n";
	cout << "-------------------------------------------------------------------\n";
	DisplayStatus();
	gEscape = 0;
}

void myFun(void * pGlobalCtx, void * pLocalCtx) {

		tConfigData* param = (tConfigData*)pLocalCtx;

	Sort(param);
	DisplayTasks(param->uTask_data->uIndex, param->uTask_data->uTask);
	gEscape = 0;
}

void  GetTaskList(void * pGlobalCtx, void * pLocalCtx) {

		tConfigData* list = new tConfigData[3];
		tParamData* data	= (tParamData*)pLocalCtx;

	system("CLS");
	ConsoleMenu n;
	n.SetTitle("Task List");
	for (int i = 0; i < data->uSortInd; ++i) {

		list[i].uTask_data = data;
		list[i].uInd = i;
		n.AddOption(list[i].uTask_data->uSort[i].uStr, myFun, &list[i]);
	}
	n.Display();
}

int main()
{

		HANDLE		hFile;
		TaskData*	task		= new TaskData[TBUFFSIZE];
		SortData*	sort_data	= new SortData[3];
		int			index;
		int			sort_index	= 0;

	InitializeCriticalSection(&gLock);
	hFile = CreateFileA("../src/config_file.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	CheckHandle(hFile);
	ConsoleMenu m;

	index = Parser(hFile, task, sort_data, sort_index);

	tParamData* param = new tParamData();
	param->uIndex = index;
	param->uTask = task;
	param->uSortInd = sort_index;
	param->uSort = sort_data;

	m.SetTitle("Task Scheduler");
	m.AddOption("Task Status", TaskStatus);
	m.AddOption("List of Tasks", GetTaskList, param);
	m.Display();

	return 0;
}

