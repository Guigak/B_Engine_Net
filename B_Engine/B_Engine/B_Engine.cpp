// B_Engine.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "B_Engine.h"
#include "ConnectServer.h"      //TODO: 테스트용 나중에 지우기 (2019180031)

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

bool g_bConsole = false;
bool g_bActive = true;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    MyRegisterClass(hInstance);

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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, dwStyle,
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
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
        printf("커맨드 창 종료 - exit\n");
        printf("프로그램 종료 - quit\n");

        scanf_s("%s", cmd, (int)sizeof(cmd));

        if (!strcmp(cmd, "connect")) {
            // TODO: 테스트용 나중에 지우기, 2019180031 - 서버에 연결
            // 127.0.0.1    - local host
            // 25.55.51.203 - 윤석's hamachi ip
            Connect_To_Server((char*)("127.0.0.1"));
            CreateKeyInputServerSocket((char*)("127.0.0.1"));
            CreateCubeServerSocket((char*)("127.0.0.1"));
            CreateRecvPlayerDataSocket((char*)"127.0.0.1");
            CreateSendLookVectorSocket((char*)"127.0.0.1");

            // 시간 스레드 생성
            HANDLE hThread1 = CreateThread(NULL, 0, Get_Time, NULL, 0, NULL);

            // Cube Input 스레드 생성
            HANDLE hThread2 = CreateThread(NULL, 0, Get_Cube_Object_From_Server, NULL, 0, NULL);

            //
            Set_Con(true);
        }
        if (!strcmp(cmd, "exit")) {
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