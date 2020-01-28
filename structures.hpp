struct tSortAttr {

public:
	const char* uFld[2] = { "on","string" };
	int			uNum_of_fields = 2;
}gSrt;

struct tFields {

public:
	const char* uFld[3] = { "name","cmd", "per" };
	int			uNum_of_fields = 3;
}gFld;

struct tOperations {

public:
	const char* uOp[2] = { "TASK","SORT" };
	int			uNum_of_ops = 2;
}gOp;

struct tParamData {
public:
	TaskData*	uTask;
	int			uIndex;
	SortData*	uSort;
	int			uSortInd;
};

struct tConfigData
{
public:
	tParamData* uTask_data;
	int		uInd;
};