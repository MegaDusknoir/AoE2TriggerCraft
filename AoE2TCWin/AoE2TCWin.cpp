// AoE2TCWin.cpp : 定义应用程序的入口点。
//

#include <map>
#include "framework.h"
#include "AoE2TCWin.h"
#include "D:\Age of Empires\AoE2TriggerCraft\aoe2scenario.h"

#define MAX_LOADSTRING 100

using namespace AoE2ScenarioNamespace;
AoE2Scenario Scen;

WCHAR szScenName[MAX_LOADSTRING];
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

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
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AOE2TCWIN, szWindowClass, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_FILE_TYPE, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_ALL_TYPE, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_OPEN, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_SAVE, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_SAVE_SUCCESS, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_SAVE_FAIL, stMap, MAX_LOADSTRING);
    LoadStringMap(hInstance, IDS_OPEN_FAIL, stMap, MAX_LOADSTRING);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
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
                                MessageBox(NULL, pszFilePath, stMap[IDS_OPEN], MB_OK);
                                // set status bar text
                                wchar_t titleBuffer[100];
                                auto filename = getFilenameFromPath(pszFilePath);
								wnsprintf(szScenName, sizeof(szScenName) / sizeof(wchar_t), L"%s", filename);
								wnsprintf(titleBuffer, sizeof(titleBuffer) / sizeof(wchar_t),
									L"%s - %s", szTitle, szScenName);
                                SetWindowText(hWnd, titleBuffer);
                            }
                            catch (std::exception& ex)
                            {
                                MessageBox(NULL, stMap[IDS_OPEN_FAIL], stMap[IDS_OPEN], MB_ICONWARNING);
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
