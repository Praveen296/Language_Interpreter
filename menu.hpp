#define MOP_BLOCK_SIZE  10

typedef void(*MenuCB) (void * pGlobalCtx, void * pLocalCtx);

struct tMenuOption {
	char *  uString;
	MenuCB  uCBFunc;
	void *  uCtx;
};

enum eMenuKey {
	MENU_KEY_UP,
	MENU_KEY_DOWN,
	MENU_KEY_ENTER,
	MENU_KEY_ESC,
};

class ConsoleMenu {

public:
	ConsoleMenu();
	~ConsoleMenu();

	void        SetTitle(const char * pTitle);
	void        AddOption(const char * pString, MenuCB pCBFunc, void * pCtx = nullptr);
	void        Display();

private:
	char *          vTitle;
	tMenuOption **  vOptions;
	int             vMaxOptions;
	int             vOptionCount;
	int             vCurrOption;
	void *          vGlobalCtx;
	bool            vShouldContinue;


	void            GrowOptions();
	void            DuplicateString(char *& pDest, const char * pSrc);
	void            DisplayTitle();
	void            DisplayOptions();
	eMenuKey        GetMenuInput();
	void            FreeMenuOption(tMenuOption * pOption);

	static    void            MenuExitFunc(void * pGlobalCtx, void * pLocalCtx);
};

ConsoleMenu::ConsoleMenu()
{
	vTitle = nullptr;
	vMaxOptions = 0;
	vOptionCount = 0;
	vCurrOption = 0;
	vOptions = nullptr;
	vGlobalCtx = nullptr;
	vShouldContinue = true;

	GrowOptions();
}

ConsoleMenu::~ConsoleMenu()
{
	free(vTitle);
	for (int idx = 0; idx < vOptionCount; idx++) {
		FreeMenuOption(vOptions[idx]);
	}
	free(vOptions);
}

void ConsoleMenu::GrowOptions()
{
	tMenuOption ** newarray = (tMenuOption **)malloc(sizeof(tMenuOption *) * (vMaxOptions + MOP_BLOCK_SIZE));

	memcpy(newarray, vOptions, vMaxOptions * sizeof(tMenuOption));
	free(vOptions);

	vOptions = newarray;
	vMaxOptions += MOP_BLOCK_SIZE;
}


void ConsoleMenu::DuplicateString(char *& pDest, const char * pSrc)
{
	free(pDest);
	pDest = (char *)malloc(strlen(pSrc) + 1);
	strcpy(pDest, pSrc);
}

void ConsoleMenu::SetTitle(const char * pTitle)
{
	DuplicateString(vTitle, pTitle);
}

void ConsoleMenu::AddOption(const char * pString, MenuCB pCBFunc, void * pCtx)
{
	tMenuOption * option = (tMenuOption *)malloc(sizeof(tMenuOption));

	option->uCtx = pCtx;
	option->uCBFunc = pCBFunc;
	option->uString = nullptr;
	DuplicateString(option->uString, pString);

	if (vOptionCount == vMaxOptions)
		GrowOptions();

	vOptions[vOptionCount] = option;

	vOptionCount++;
}

void ConsoleMenu::DisplayTitle()
{
	printf("%s\n", vTitle);
	for (int idx = 0; idx < strlen(vTitle); ++idx) {
		printf("=");
	}
}


void ConsoleMenu::MenuExitFunc(void * pGlobalCtx, void * pLocalCtx)
{
	((ConsoleMenu *)(pLocalCtx))->vShouldContinue = false;
}

void ConsoleMenu::FreeMenuOption(tMenuOption * pOption)
{
	free(pOption->uString);
	free(pOption);
}

eMenuKey ConsoleMenu::GetMenuInput()
{
	fflush(stdin);
	while (true) {
		switch (_getch()) {
		case 72:    return MENU_KEY_UP;
		case 80:    return MENU_KEY_DOWN;
		case 13:    return MENU_KEY_ENTER;
		case 27:    return MENU_KEY_ESC;
		}
	}
}

void ConsoleMenu::DisplayOptions()
{
	printf("\n\n");
	printf("Options\n-------\n");

	for (int idx = 0; idx < vOptionCount; idx++) {
		printf("  %s%s\n", (idx == vCurrOption) ? "->" : "  ", vOptions[idx]->uString);
	}
}

void ConsoleMenu::Display()
{
	AddOption("Exit", MenuExitFunc, this);
	vShouldContinue = true;
	vCurrOption = 0;
	while (vShouldContinue) {
		system("cls");
		DisplayTitle();
		DisplayOptions();

		switch (GetMenuInput()) {
		case MENU_KEY_UP:
			if (vCurrOption == 0)
				vCurrOption = vOptionCount - 1;
			else
				vCurrOption--;
			break;

		case MENU_KEY_DOWN:

			if (vCurrOption == vOptionCount - 1)
				vCurrOption = 0;
			else
				vCurrOption++;
			break;

		case MENU_KEY_ENTER:
			system("CLS");
			DisplayTitle();
			vOptions[vCurrOption]->uCBFunc(vGlobalCtx, vOptions[vCurrOption]->uCtx);
			break;

		case MENU_KEY_ESC:
			vShouldContinue = false;
			break;
		}
	}

	FreeMenuOption(vOptions[vOptionCount - 1]);
	vOptionCount--;
}
