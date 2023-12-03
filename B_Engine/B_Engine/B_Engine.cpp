// B_Engine.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "B_Engine.h"
#include <list>
#include "ConnectServer.h"      //TODO: 테스트용 나중에 지우기 (2019180031)
#include "sstream"
#include "string"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

bool g_bConsole = false;
bool g_bActive = true;

HWND hWnd;

//
HWND hTimer_Wnd;

LRESULT CALLBACK Timer_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Udt_Timer(int nNumber);

//=================== 채팅창 관련 시작 ===================
#define CHAT_BOX_WIDTH 500
#define CHAT_BOX_HEIGHT 300
#define MAX_LAST_CHAT 30

class ChatString
{
public:
    ChatString() {}
    ~ChatString(){}
    ChatString(int pn, std::string cd)
    {
        playerNumber = pn;
        for (int i = 0; i < cd.size(); ++i)
        {
            chatData[i] = cd[i];
        }
        chatData[cd.size()] = '\0';
    };
public:
    int playerNumber{};       // -1 -> 커맨드 채팅, 0 -> 0번 플레이어, 1 -> 1번 플레이어,
	                        // 2  -> 2번 플레이어, ..., MAX_PLAYER_COUNT-> 오프라인시 채팅
    char chatData[CHATMAXBUFFER + 1]{0,};
};
HWND childHWND;
bool PressingReturn = false;
std::string InputString = "";
std::list<ChatString> LastChatData;
int ChatBoxWheel{};
void SetChatBoxOpenClose(WPARAM wParam, UINT uMsg);
void DoCommandAction();
void AddLastChatData(int playerNumber, std::string);
DWORD WINAPI RecvChatData(LPVOID arg);
//=================== 채팅창 관련 끝 ===================

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK ChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Framework
CFramework gFramework;

// console function
void Prcs_Console_Cmd();

DWORD WINAPI Prcs_Console(LPVOID argument) {
    AllocConsole();
    SetConsoleTitle(TEXT("커맨드 창"));

    //
    _tfreopen(_T("CONOUT$"), _T("w"), stdout);
    _tfreopen(_T("CONIN$"), _T("r"), stdin);
    _tfreopen(_T("CONERR$"), _T("w"), stderr);
    _tsetlocale(LC_ALL, _T(""));

    //
    Prcs_Console_Cmd();

    //
    HWND hWndConsole = GetConsoleWindow();
    ShowWindow(hWndConsole, SW_HIDE);

    FreeConsole();

    return 0;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BENGINE, szWindowClass, MAX_LOADSTRING);
    //MyRegisterClass(hInstance);
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BENGINE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    //wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BENGINE);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    //
    wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
    wcex.lpfnWndProc = Timer_Proc;
    wcex.lpszClassName = TEXT("Timer_Wnd");
    wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_MENU);
    RegisterClassExW(&wcex);

    // TODO: ChildWindow 만들고서 생성하기
    
    wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
    wcex.lpfnWndProc = ChildProc;
    wcex.lpszClassName = TEXT("ChildCls");
    wcex.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    RegisterClassExW(&wcex);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BENGINE));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (1)
    {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }

            if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
        else {
            gFramework.Adavance_Frame();
        }

        //
        if (GetActiveWindow() == GetForegroundWindow()) {
            ShowWindow(hTimer_Wnd, SW_SHOW);
        }
        else {
            ShowWindow(hTimer_Wnd, SW_HIDE);
        }
    }
    gFramework.OnDestroy();

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BENGINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    //wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BENGINE);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);


}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;

   RECT rcWnd = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
   AdjustWindowRect(&rcWnd, dwStyle, FALSE);

	hWnd = CreateWindowW(szWindowClass, szTitle, dwStyle,
      CW_USEDEFAULT, CW_USEDEFAULT, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   gFramework.OnCreate(hInstance, hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

#ifdef _WITH_SWAPCHAIN_FULLSCREEN_STATE
   gFramework.Chg_SwapChain_State();
#endif

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

#define TIMER_RECT_WIDTH  FRAME_BUFFER_WIDTH / 10
#define TIMER_RECT_HEIGHT FRAME_BUFFER_HEIGHT / 15

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
	    {
        RECT rect;
        GetWindowRect(hWnd, &rect);
        childHWND = CreateWindow(TEXT("ChildCls"), NULL, WS_POPUP | WS_VISIBLE,
            rect.left + 10, rect.bottom - CHAT_BOX_HEIGHT-10, CHAT_BOX_WIDTH, CHAT_BOX_HEIGHT, hWnd, (HMENU)NULL, hInst, NULL);
        ShowWindow(childHWND, SW_HIDE);

        //
        hTimer_Wnd = CreateWindow(TEXT("Timer_Wnd"), NULL, WS_POPUP | WS_VISIBLE, rect.left + FRAME_BUFFER_WIDTH * 9 / 20, rect.top + 40, TIMER_RECT_WIDTH, TIMER_RECT_HEIGHT, hWnd, (HMENU)NULL, hInst, NULL);
	    }
        
        break;
    case WM_MOVE:
	    {
        RECT rect;
        GetWindowRect(hWnd, &rect);
        SetWindowPos(childHWND, HWND_TOPMOST, rect.left + 10, rect.bottom - CHAT_BOX_HEIGHT - 10, CHAT_BOX_WIDTH, CHAT_BOX_HEIGHT, NULL);

        SetWindowPos(hTimer_Wnd, HWND_TOPMOST, rect.left + FRAME_BUFFER_WIDTH * 9 / 20, rect.top + 40, TIMER_RECT_WIDTH, TIMER_RECT_HEIGHT, NULL);
	    }
        
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE :
    case WM_LBUTTONDOWN :
    case WM_LBUTTONUP :
    case WM_RBUTTONDOWN :
    case WM_RBUTTONUP :
    case WM_MOUSEMOVE :
    case WM_KEYDOWN :
    case WM_KEYUP :
        if (wParam == VK_F8 && !g_bConsole) {
            HANDLE hThread;

            hThread = CreateThread(NULL, 0, Prcs_Console, NULL, 0, NULL);

            if (hThread == NULL) {
                OutputDebugStringA("Create console fail\n");
            }
            else {
                CloseHandle(hThread);
            }

            g_bConsole = true;
        }

        SetChatBoxOpenClose(wParam, message);

        gFramework.Prcs_Msg_Wnd(hWnd, message, wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    if (!g_bActive) {
        PostQuitMessage(0);
    }

    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}



// console function
void Prcs_Console_Cmd() {
    char cmd[10] = { 0 };

    while (1) {
        printf("\n커맨드를 입력해주세요.\n");
        printf("서버 연결 - connect\n");
        printf("큐브 클리어 - clear\n");
        printf("커맨드 창 종료 - exit\n");
        printf("프로그램 종료 - quit\n");

        scanf_s("%s", cmd, (int)sizeof(cmd));

        if (!strcmp(cmd, "connect")) {

            Connect_To_Server(IP_ADDRESS);
            CreateKeyInputServerSocket(IP_ADDRESS);
            CreateCubeServerSocket(IP_ADDRESS);
            CreateRecvPlayerDataSocket(IP_ADDRESS);

            // 시간 스레드 생성
            HANDLE hThread1 = CreateThread(NULL, 0, Get_Time, NULL, 0, NULL);

            // Cube Input 스레드 생성
            HANDLE hThread2 = CreateThread(NULL, 0, Get_Cube_Object_From_Server, NULL, 0, NULL);

            //
            gFramework.Clr_Cube_Objects();
            Set_Con(true);
        }
        else if (!strcmp(cmd, "clear")) {
            gFramework.Clr_Cube_Objects();
        }
        else if (!strcmp(cmd, "exit")) {
            g_bConsole = false;
            break;
        }
        else if (!strcmp(cmd, "quit")) {
            g_bConsole = false;
            g_bActive = false;
            break;
        }
    }
}
HDC hdc;
HDC memdc;
HBRUSH blackBrush, whiteBrush;
LRESULT CALLBACK ChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //if (!ShowChatBox) return DefWindowProc(hWnd, uMsg, wParam, lParam);
    PAINTSTRUCT ps;
    
    switch (uMsg) {
    case WM_CREATE:
        blackBrush = CreateSolidBrush(RGB(50, 50, 50));
        whiteBrush = CreateSolidBrush(RGB(245, 245, 245));
        break;
    case WM_PAINT:
	    {
			hdc = BeginPaint(hWnd, &ps);
            SelectObject(hdc, whiteBrush);
            RECT inputrect = { 0 ,CHAT_BOX_HEIGHT - 30,CHAT_BOX_WIDTH,CHAT_BOX_HEIGHT };
            FillRect(hdc, &inputrect, whiteBrush);
            inputrect.left += 10;
            SetTextColor(hdc, RGB(0, 0, 0));
            DrawTextA(hdc, InputString.c_str(), InputString.size(), &inputrect, DT_LEFT|DT_VCENTER|DT_SINGLELINE);
			//TextOutA(hdc,10,CHAT_BOX_HEIGHT-20, InputString.c_str(), InputString.size());

    		SelectObject(hdc, blackBrush);
            RECT outputrect = { 0,0,CHAT_BOX_WIDTH,CHAT_BOX_HEIGHT - 30 };
            FillRect(hdc, &outputrect, blackBrush);
            outputrect.top = CHAT_BOX_HEIGHT - 60;
            outputrect.left += 10;
            if(!LastChatData.empty())
            {
                auto plist = LastChatData.begin();
                for (int i = 0; i < ChatBoxWheel; ++i) {
                    if (ChatBoxWheel + CHAT_BOX_HEIGHT / 30 >= MAX_LAST_CHAT) break;
                    ++plist;
                }

                for(plist; plist!=LastChatData.end(); ++plist)
                {
                    COLORREF text_color;
                    switch(plist->playerNumber)
                    {
                    case -1: // 시스템
                        text_color = RGB(255, 0, 255);
                        break;
                    case 0:
                        text_color = RGB(255,0,0);
                        break;
                    case 1:
                        text_color = RGB(0, 255, 0);
                        break;
                    case 2:
                        text_color = RGB(0,0,255);
                        break;
                    case PLAYER_MAX_NUMBER:
                        text_color = RGB(0,0,0);
                        break;
                    default:
                        text_color = RGB(0, 255, 255);
                        break;
                    }
                    SetTextColor(hdc, text_color);
                    std::string cd = plist->chatData;
                	DrawTextA(hdc, cd.c_str(),cd.size(), &outputrect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
                    
                    outputrect.top -= 30;
                    outputrect.bottom -= 30;
                }
               
            }
            

            
    		EndPaint(hWnd, &ps);
	    }
        
        break;
    case WM_KEYDOWN:
	    {
            if(wParam==VK_BACK)
            {
	            if(!InputString.empty())
	            {
                    InputString.pop_back();
	            }
            }else
            {
                if(wParam!=VK_RETURN&&wParam!=229 && wParam!=229 && InputString.size()<= CHATMAXBUFFER-10)//엔터, 한영키
                {
                    
                    if (wParam == ' ') InputString.push_back(' ');
                    else if (wParam == 222) InputString.push_back('\"');
                    else if (wParam == 190)InputString.push_back('.');
                    else if (wParam == 191) InputString.push_back('/');
                    else {
                        char c = (char)wParam;
                        if (('A'<=c && c<='Z') || ('0'<=c && c<='9'))
                        {
                            if (isalpha(c)) c += ('a' - 'A');
                            InputString.push_back(c);
                        }
                    }
                }
            }
	    }
        SetChatBoxOpenClose(wParam, uMsg);
        InvalidateRect(hWnd, NULL, true);
        break;
    case WM_KEYUP:
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
        SetChatBoxOpenClose(wParam, uMsg);
        InvalidateRect(hWnd, NULL, true);
        break;
    case WM_MOUSEWHEEL:
        if(GET_WHEEL_DELTA_WPARAM(wParam)<0)
        {
            if (ChatBoxWheel > 0) --ChatBoxWheel;
        }
        else
        {
            if (ChatBoxWheel + CHAT_BOX_HEIGHT / 30 < LastChatData.size()) ++ChatBoxWheel;
        }
        InvalidateRect(hWnd, NULL, true);
        break;
    case WM_DESTROY:
        DeleteObject(blackBrush);
        DeleteObject(whiteBrush);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void SetChatBoxOpenClose(WPARAM wParam, UINT uMsg)
{
    if (wParam == VK_RETURN)
    {
        if (uMsg == WM_KEYDOWN)
        {
            if (!PressingReturn)
            {
                if (!GetShowChatBox())
                {
                    ChatBoxWheel = 0;
                    ShowWindow(childHWND, SW_SHOW);
                    SetShowChatBox(true);
                }
                else
                {
                    //엔터키 다시 눌러서 채팅창 닫기
                    ShowWindow(childHWND, SW_HIDE);
                    SetShowChatBox(false);

                    if(!InputString.empty())
                    {
                        // 명령어 입력시
	                    if(InputString[0] == '/')
	                    {
							DoCommandAction();
	                    }
                        else //명령어가 아니면 채팅 서버로 전송
                        {
                            if (Get_Con())
                            {
                                ChatString chatdata;
                                chatdata.playerNumber = GetPlayerNumber();
                                std::string tempstring = std::string("[") + std::to_string(GetPlayerNumber()) + std::string("] - ") + InputString;
                                for(int i=0; i<tempstring.size(); ++i)
                                {
                                    chatdata.chatData[i] = tempstring[i];
                                }
                                chatdata.chatData[tempstring.size()] = '\0';
                                int retval = send(GetChatDataSocket(), (char*)&chatdata, sizeof(ChatString), 0);
                                if(retval==INVALID_SOCKET)
                                {
                                    DisconnectServer();
                                    return;
                                }
                            }
                            else 
                                AddLastChatData(PLAYER_MAX_NUMBER, InputString);
                        }
                    }
                    InputString.clear();
                }
                PressingReturn = true;
            }
        }
        else
        {
            PressingReturn = false;
        }
    }
    InvalidateRect(childHWND, NULL, true);
}
void ClearCube()
{
    gFramework.Clr_Cube_Objects();
    std::string cd = std::string("[시스템] \"") + std::string("모든 블럭을 초기화 하였습니다.");
    AddLastChatData(-1, cd);
}
void DoCommandAction()
{
    InputString = { InputString.begin() + 1, InputString.end() };
    std::vector<std::string> words;
    std::stringstream ss(InputString);
    std::string word;
    while (std::getline(ss, word, ' ')) words.push_back(word);
    if(words.size() == 0)
    {
        std::string chatdata = "[시스템] 잘못된 명령어 입력입니다.";
        AddLastChatData(-1, chatdata);
        return;
    }
    if (words[0] == "connect")
    {
        bool bCanConnect = Connect_To_Server(words[1].c_str());
        if(bCanConnect)
        {
            ClearCube();
            CreateKeyInputServerSocket(words[1].c_str());
            CreateCubeServerSocket(words[1].c_str());
            CreateRecvPlayerDataSocket(words[1].c_str());
            CreateChatDataSocket(words[1].c_str());
            // 시간 스레드 생성
            HANDLE hThread1 = CreateThread(NULL, 0, Get_Time, NULL, 0, NULL);

            // Cube Input 스레드 생성
            HANDLE hThread2 = CreateThread(NULL, 0, Get_Cube_Object_From_Server, NULL, 0, NULL);
            // 채팅 스레드 생성
            HANDLE hThread3 = CreateThread(NULL, 0, RecvChatData, NULL, 0, NULL);
            //
            gFramework.Clr_Cube_Objects();
            Set_Con(true);

            std::string cd = std::string("[시스템] \"") + words[1] + std::string("에 접속하였습니다.");
            AddLastChatData(-1, cd);
        }
        else
        {
            std::string cd = std::string("[시스템] 연결하려는 서버가 적절치 않거나 오프라인 상태입니다.");
            AddLastChatData(-1, cd);
        }
        
    }
    else if (words[0] == "clear") {
        ClearCube();
    }
    else if (words[0] == "settimer") {
        Udt_Timer(std::stoi(words[1]));
    }
    else if (words[0] == "exit") {
        g_bConsole = false;
    }
    else if (words[0] == "quit") {
        g_bConsole = false;
        g_bActive = false;
    }else if(words[0] == "cs") //connect self server
    {
        bool bCanConnect = Connect_To_Server("127.0.0.1");
        if (bCanConnect)
        {
            ClearCube();
            CreateKeyInputServerSocket("127.0.0.1");
            CreateCubeServerSocket("127.0.0.1");
            CreateRecvPlayerDataSocket("127.0.0.1");
            CreateChatDataSocket("127.0.0.1");
            // 시간 스레드 생성
            HANDLE hThread1 = CreateThread(NULL, 0, Get_Time, NULL, 0, NULL);

            // Cube Input 스레드 생성
            HANDLE hThread2 = CreateThread(NULL, 0, Get_Cube_Object_From_Server, NULL, 0, NULL);
            // 채팅 스레드 생성
            HANDLE hThread3 = CreateThread(NULL, 0, RecvChatData, NULL, 0, NULL);
            //
            gFramework.Clr_Cube_Objects();
            Set_Con(true);


            std::string cd = std::string("[시스템] \"") + "127.0.0.1" + std::string("에 접속하였습니다.");
            AddLastChatData(-1, cd);
        }
        else
        {
            std::string cd = std::string("[시스템] 연결하려는 서버가 적절치 않습니다.");
            AddLastChatData(-1, cd);
        }
    }else
    {
        std::string chatdata = "[시스템] 잘못된 명령어 입력입니다.";
        AddLastChatData(-1, chatdata);
    }
}
/*
std::string InputString = "";
std::queue<ChatString> LastChatData;
int ChatBoxWheel{};
void SetChatBoxOpenClose(WPARAM wParam, UINT uMsg);
*/
void AddLastChatData(int playerNumber, std::string chatdata)
{
    if(LastChatData.size() >= MAX_LAST_CHAT)
    {
        LastChatData.pop_back();
    }
    ChatBoxWheel = 0;
    LastChatData.push_front(ChatString(playerNumber, chatdata));


    InvalidateRect(childHWND, NULL, true);
}


DWORD WINAPI RecvChatData(LPVOID arg)
{
    int retval = 0;
    ChatString chatdata;

    while (1) {
        retval = recv(GetChatDataSocket(), (char*)&chatdata, sizeof(ChatString), MSG_WAITALL);
        if (retval == INVALID_SOCKET)
        {
            DisconnectServer();
            return -1;
        }
        AddLastChatData(chatdata.playerNumber, chatdata.chatData);
        InvalidateRect(childHWND, NULL, true);
    }

    // 소켓 닫기
    closesocket(GetChatDataSocket());
    // 윈속 종료
    WSACleanup();
    return 0;
}

//
HBRUSH hbBackground;

static int nTimer = 0;

LRESULT CALLBACK Timer_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HDC hDC;
    PAINTSTRUCT ps;

    switch (uMsg) {
    case WM_CREATE:
        hbBackground = CreateSolidBrush(RGB(128, 128, 128));

        SetTimer(hWnd, 1, 100, NULL);
        break;
    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);
        SelectObject(hDC, hbBackground);

        RECT rect;
        GetWindowRect(hWnd, &rect);

        RECT rcRender = { 4, 4, TIMER_RECT_WIDTH - 4, TIMER_RECT_HEIGHT - 4 };
        //RECT rcRender = { 2, 2, TIMER_RECT_WIDTH - 4, TIMER_RECT_HEIGHT - 4 };
        FillRect(hdc, &rcRender, hbBackground);
        SetBkMode(hDC, TRANSPARENT);

        char number[10] = "";

        if (Get_Con()) {
            itoa(Get_Now_Time(), number, 10);
        }
        else {
            sprintf(number, "---");
        }

        DrawTextA(hdc, number, -1, &rcRender, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        InvalidateRect(hWnd, NULL, true);
        UpdateWindow(hWnd);

		EndPaint(hWnd, &ps);
	}
	    break;
    case WM_TIMER:
        InvalidateRect(hWnd, NULL, true);
        UpdateWindow(hWnd);
        break;
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		break;
	case WM_MOUSEWHEEL:
		break;
	case WM_DESTROY:
		DeleteObject(hbBackground);
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void Udt_Timer(int nNumber) {
    nTimer = nNumber;

    SendMessage(hTimer_Wnd, WM_PAINT, NULL, NULL);
}