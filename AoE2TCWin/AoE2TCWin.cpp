// AoE2TCWin.cpp : 定义应用程序的入口点。
//
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment (lib, "comctl32")

#include "framework.h"
#include "AoE2TCWin.h"
#include "../aoe2scenario.h"
#include "view/editors.h"
#include <map>

using namespace AoE2ScenarioNamespace;
AoE2Scenario Scen;
AoEJsonData GameStr;

WCHAR szScenName[MAX_LOADSTRING];
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND hBar;
WCHAR szEditorWindowClass[MAX_LOADSTRING] = L"EditorWindowClass";
HWND hEditor, hEditorSheet;
WCHAR szViewerWindowClass[MAX_LOADSTRING] = L"ViewerWindowClass";
HWND hViewer;
WCHAR szInfoWindowClass[MAX_LOADSTRING] = L"InfoWindowClass";
HWND hInfo;

class StringMap
{
public:
    void alloc(UINT uID, int cchBufferMax)
    {
        unique_ptr<WCHAR[]> sz(new WCHAR[cchBufferMax]);
        map[uID] = std::move(sz);
    }
    WCHAR* operator[](UINT uID)
    {
        if (map.find(uID) != map.end())
        {
            return map[uID].get();
        }
        else
        {
            throw std::runtime_error("Invalid string index");
        }
    }
private:
    std::map<size_t, unique_ptr<WCHAR[]>> map;
};
StringMap stMap;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void OpenFile(HWND hWnd);
void SaveFile(HWND hWnd);

void GameDataJsonLoad(void)
{
    WCHAR szDir[512];
    GetCurrentDirectory(512, szDir);
    AoEJsonRead(GameStr).json_load((std::wstring(szDir) + L"/string.json").c_str());
}

LRESULT CALLBACK EditorWndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = 0;
    switch (msg)
    {
    case TC_LOAD:
        SendMessage((HWND)SendMessage(hEditorSheet, PSM_GETCURRENTPAGEHWND, 0, 0), TC_LOAD, 0, 0);
        break;
    default:
        ret = DefWindowProc(window, msg, wParam, lParam);
    }

    return ret;
}
ATOM MakeEditorClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = EditorWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szEditorWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}
HWND CreateEditorWnd(HWND hParent, int x)
{
    static bool registered = false;

    // WTF is this for? Yeah, I've read the docs, but the question remains.
    HINSTANCE instance = GetModuleHandle(NULL);

    if (!registered)
    {
        MakeEditorClass(instance);
        registered = true;
    }

    RECT rect;
    GetClientRect(hParent, &rect);
    HWND hEditor = CreateWindowEx(0, szEditorWindowClass, L"编辑器区",
        WS_OVERLAPPED | WS_BORDER | WS_CHILD | WS_VISIBLE,
        0, 0, x, rect.bottom - rect.top,
        hParent, nullptr, instance, nullptr);

    if (!hEditor)
    {
        return FALSE;
    }

    return hEditor;
}


LRESULT CALLBACK InfoWndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = 0;
    switch (msg)
    {
    default:
        ret = DefWindowProc(window, msg, wParam, lParam);
    }

    return ret;
}
ATOM MakeInfoClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = InfoWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szInfoWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}
HWND CreateInfoWnd(HWND hParent, int y)
{
    static bool registered = false;

    // WTF is this for? Yeah, I've read the docs, but the question remains.
    HINSTANCE instance = GetModuleHandle(NULL);

    if (!registered)
    {
        MakeInfoClass(instance);
        registered = true;
    }

    RECT rect;
    GetClientRect(hParent, &rect);
    RECT rectEditor;
    GetClientRect(hEditor, &rectEditor);
    HWND hInfo = CreateWindowEx(0, szInfoWindowClass, L"详细信息区",
        WS_OVERLAPPED | WS_BORDER | WS_CHILD | WS_VISIBLE,
        rectEditor.right + 2, y, rect.right - (rectEditor.right + 2), rect.bottom - y,
        hParent, nullptr, instance, nullptr);

    if (!hInfo)
    {
        return FALSE;
    }

    return hInfo;
}

LRESULT CALLBACK ViewerWndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = 0;
    switch (msg)
    {
    default:
        ret = DefWindowProc(window, msg, wParam, lParam);
    }

    return ret;
}
ATOM MakeViewerClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = ViewerWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szViewerWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}
HWND CreateViewerWnd(HWND hParent)
{
    static bool registered = false;

    // WTF is this for? Yeah, I've read the docs, but the question remains.
    HINSTANCE instance = GetModuleHandle(NULL);

    if (!registered)
    {
        MakeViewerClass(instance);
        registered = true;
    }

    RECT rect;
    RECT rectEditor;
    RECT rectInfo;
    GetClientRect(hParent, &rect);
    GetClientRect(hEditor, &rectEditor);
    GetClientRect(hInfo, &rectInfo);
    HWND hViewer = CreateWindowEx(0, szViewerWindowClass, L"详细信息区",
        WS_OVERLAPPED | WS_BORDER | WS_CHILD | WS_VISIBLE,
        rectEditor.right + 2, 0, rect.right - (rectEditor.right + 2), rect.bottom - (rectInfo.bottom + 2),
        hParent, nullptr, instance, nullptr);

    if (!hViewer)
    {
        return FALSE;
    }

    return hViewer;
}


const wchar_t* getFilenameFromPath(const wchar_t* path)
{
	const wchar_t* ret;

	ret = wcsrchr(path, L'\\') + 1;
	if (!ret)
	{
		ret = wcsrchr(path, L'/') + 1;

		if (!ret)
			ret = path;
	}

	return ret;
}

void LoadStringMap(HINSTANCE hInstance, UINT uID, StringMap& sMap, int cchBufferMax)
{
    sMap.alloc(uID, cchBufferMax);
    LoadStringW(hInstance, uID, stMap[uID], cchBufferMax);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    GameDataJsonLoad();
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AOE2TCWIN, szWindowClass, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_FILE_TYPE, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_ALL_TYPE, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_OPEN, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_SAVE, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_SAVE_SUCCESS, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_SAVE_FAIL, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_OPEN_FAIL, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_WELCOME, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_OPEN_SUCCESS, stMap, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AOE2TCWIN));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AOE2TCWIN));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_AOE2TCWIN);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowEx(
       WS_EX_ACCEPTFILES,                              // Optional window styles.
	   szWindowClass,                     // Window class
	   szTitle,    // Window text
	   WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),            // Window style

	   // Size and position
	   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

	   nullptr,       // Parent window    
	   nullptr,       // Menu
	   hInstance,  // Instance handle
	   nullptr        // Additional application data
   );

   if (!hWnd)
   {
	   return FALSE;
   }

   hEditor = CreateEditorWnd(hWnd, 300);
   if (!hEditor)
   {
	   return FALSE;
   }
   hInfo = CreateInfoWnd(hWnd, 500);
   if (!hInfo)
   {
       return FALSE;
   }
   hViewer = CreateViewerWnd(hWnd);
   if (!hViewer)
   {
       return FALSE;
   }
   hEditorSheet = MakeEditorSheet(hInstance);
   hBar = CreateWindow(STATUSCLASSNAME, stMap[IDS_WELCOME],
	   WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
       hWnd, NULL, hInstance, NULL);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
void OnSize(HWND hWnd)
{
    RECT rect;
    RECT rectEditor;
    RECT rectInfo;
    RECT rectViewer;
    GetClientRect(hWnd, &rect);
    GetClientRect(hEditor, &rectEditor);
    GetClientRect(hInfo, &rectInfo);
    GetClientRect(hViewer, &rectViewer);
    LONG Info_height = rectInfo.bottom - rectInfo.top + 2;
    LONG Editor_width = rectEditor.right - rectEditor.left + 2;
    MoveWindow(hEditor,
        0,
        0,
        Editor_width,
        rect.bottom - rect.top,
        TRUE);
    MoveWindow(hInfo,
        rectEditor.right + 2,
        rect.bottom - Info_height,
        rect.right - (rectEditor.right + 2),
        Info_height,
        TRUE);
    MoveWindow(hViewer,
        rectEditor.right + 2,
        0,
        rect.right - (rectEditor.right + 2),
        rect.bottom - (rectInfo.bottom + 2),
        TRUE);
}
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case IDM_OPEN:
				OpenFile(hWnd);
				break;
			case IDM_SAVE_TO:
                if (wcslen(szScenName))
                {
                    SaveFile(hWnd);
                }
                break;
			case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_WINDOWPOSCHANGED:
    case WM_SIZING:
        {
		    //RECT* rect = (RECT*)lParam;
            OnSize(hWnd);
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_DROPFILES:
        ;
        break;
    case TC_LOAD:
    {
        SendMessage(hEditor, TC_LOAD, 0, 0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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

void OpenFile(HWND hWnd)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		CComPtr<IFileOpenDialog> pFileOpen;

		// Create the FileOpenDialog object.
		hr = pFileOpen.CoCreateInstance(__uuidof(FileOpenDialog));
		if (SUCCEEDED(hr))
		{
            COMDLG_FILTERSPEC const rgOpenTypes[] =
			{
                { stMap[IDS_FILE_TYPE], L"*.aoe2scenario"},
				{ stMap[IDS_ALL_TYPE], L"*.*" },
            };

            hr = pFileOpen->SetFileTypes(ARRAYSIZE(rgOpenTypes), rgOpenTypes);
            if (SUCCEEDED(hr))
            {
                // Show the Open dialog box.
                hr = pFileOpen->Show(NULL);

                // Get the file name from the dialog box.
                if (SUCCEEDED(hr))
                {
                    CComPtr<IShellItem> pItem;
                    hr = pFileOpen->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        PWSTR pszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                        // Display the file name to the user.
                        if (SUCCEEDED(hr))
                        {
                            try
                            {
                                Scen.open(pszFilePath);
                                //MessageBox(NULL, pszFilePath, stMap[IDS_OPEN], MB_OK);
                                // set status bar text
                                wchar_t titleBuffer[100];
                                auto filename = getFilenameFromPath(pszFilePath);
								wnsprintf(szScenName, sizeof(szScenName) / sizeof(wchar_t), L"%s", filename);
								wnsprintf(titleBuffer, sizeof(titleBuffer) / sizeof(wchar_t),
									L"%s - %s", szTitle, szScenName);
                                SetWindowText(hWnd, titleBuffer);
                                SetWindowText(hBar, stMap[IDS_OPEN_SUCCESS]);
                                SendMessage(hWnd, TC_LOAD, 0, 0);
                            }
                            catch (std::exception& ex)
                            {
                                MessageBox(NULL, stMap[IDS_OPEN_FAIL], stMap[IDS_OPEN], MB_ICONWARNING);
                                SetWindowText(hBar, stMap[IDS_OPEN_FAIL]);
                            }
                            CoTaskMemFree(pszFilePath);
                        }
                    }
                }
            }
		}
		CoUninitialize();
	}
}

void SaveFile(HWND hWnd)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        CComPtr<IFileSaveDialog> pFileSave;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC, IID_PPV_ARGS(&pFileSave));
        if (SUCCEEDED(hr))
        {
            hr = pFileSave->SetFileName(szScenName);
            COMDLG_FILTERSPEC const rgSaveTypes[] =
            {
                { stMap[IDS_FILE_TYPE], L"*.aoe2scenario" },
            };

            hr = pFileSave->SetFileTypes(ARRAYSIZE(rgSaveTypes), rgSaveTypes);
            if (SUCCEEDED(hr))
            {
                // Show the Open dialog box.
                hr = pFileSave->Show(NULL);

                // Get the file name from the dialog box.
                if (SUCCEEDED(hr))
                {
                    CComPtr<IShellItem> pItem;
                    hr = pFileSave->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        PWSTR pszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        
                        // Display the file name to the user.
                        if (SUCCEEDED(hr))
                        {
                            try
                            {
                                Scen.save(pszFilePath);
                                MessageBox(NULL, stMap[IDS_SAVE_SUCCESS], stMap[IDS_SAVE], MB_OK);
                            }
                            catch (std::exception& ex)
                            {
                                MessageBox(NULL, stMap[IDS_SAVE_FAIL], stMap[IDS_SAVE], MB_ICONWARNING);
                            }
                            CoTaskMemFree(pszFilePath);
                        }
                    }
                }
            }
        }
        CoUninitialize();
    }
}
