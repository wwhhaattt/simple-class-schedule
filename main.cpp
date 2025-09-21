#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <sstream>
#include <iostream>
#include <locale>

// 课程表数据结构：每周每天对应的课程
std::map<int,std::vector<std::wstring>> schedule={
	{0,{L"周一",L"英语",L"物理",L"政治",L"数学",L"",L"历史",L"生物",L"语文",L"班会"}},// 周一
	{1,{L"周二",L"语文",L"物理",L"体育",L"生物",L"",L"英语",L"美术",L"数学",L"化学"}},// 周二
	{2,{L"周三",L"数学",L"英语",L"通技",L"化学",L"",L"信息",L"政治",L"物理",L"校本"}},// 周三
	{3,{L"周四",L"化学",L"数学",L"语文",L"语文",L"",L"体育",L"物理",L"英语",L"生物"}},// 周四
	{4,{L"周五",L"语文",L"英语",L"信息",L"化学",L"",L"生物",L"数学",L"历史",L"劳动"}},// 周五
	{5,{L"周六",L"休息日",L"没有课程安排"}},// 周六
	{6,{L"周日",L"休息日",L"没有课程安排"}}  // 周日
};

// 全局变量
HWND hMainWnd;
const int WINDOW_WIDTH=140;
const int WINDOW_HEIGHT=600;
const int MARGIN=20;

//wwhhaattt !
const int titlesize=30;
const int coursesize=60;
int t,today;
std::map<int,std::vector<std::wstring>>change;
std::vector<std::wstring>changeschedule;
std::wstring NarrowToWide(const std::string& narrowStr) {
	int wideLen = MultiByteToWideChar(CP_ACP, 0, narrowStr.c_str(), -1, NULL, 0);
	std::wstring wideStr(wideLen, L'\0');
	MultiByteToWideChar(CP_ACP, 0, narrowStr.c_str(), -1, &wideStr[0], wideLen);
	return wideStr;
}
void readchange()
{
	while(1)
	{
		int x=0;
		changeschedule.clear();
		std::wstring c;
		std::string sc;
		std::cin>>sc;
		c=NarrowToWide(sc);
		
		printf("循环中\n");
		std::wcout<<c<<std::endl;
		
		if(sc=="0")
		{
			return;
		}
		for(int i=0;i<8;i++)
		{
			x*=10;
			x+=c[i]-'0';
		}
		for(int i=0;i<9;i++)
		{
			std::cin>>sc;
			c=NarrowToWide(sc);
			changeschedule.push_back(c);
			if(i==4)changeschedule.push_back(L"");
		}
		change.emplace(x,changeschedule);
	}
}

void encodetime()
{
	time_t now=time(0);
	tm* ltm=localtime(&now);
	today=(ltm->tm_year+1900)*10000+(ltm->tm_mon+1)*100+ltm->tm_mday;
}


// 获取当前是星期几 (0=周日,1=周一,...,6=周六)
int GetDayOfWeek() {
	time_t now=time(0);
	tm* ltm=localtime(&now);
	return ltm->tm_wday; // 注意: Sunday=0,Monday=1,...,Saturday=6
}

// 调整星期索引以匹配我们的课程表 (0=周一,6=周日)
int GetScheduleIndex() {
	int day=GetDayOfWeek();
	// 将周日(0)转换为6，周一(1)转换为0，以此类推
	return (day+6)%7;
}

// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	switch (message) {
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc=BeginPaint(hWnd,&ps);
			
			// 设置背景透明
			SetBkMode(hdc,TRANSPARENT);
			
			// 创建半透明背景
			RECT rect;
			GetClientRect(hWnd,&rect);
			HBRUSH hBrush=CreateSolidBrush(RGB(240,240,240));
			FillRect(hdc,&rect,hBrush);
			DeleteObject(hBrush);
			
			// 设置文本颜色
			SetTextColor(hdc,RGB(0,0,0));
			
			
			//读取修改
			freopen("change.txt","r",stdin);
			encodetime();
			readchange();
			
			// 获取当前课程
			std::vector<std::wstring> todayCourses;
			if(change.find(today)==change.end()){//未被修改
				int dayIndex=GetScheduleIndex();
				todayCourses=schedule[dayIndex];
			}
			else{//被修改了
				auto it=change.find(today);
				todayCourses=it->second;
			}
			/*
			int dayIndex=GetScheduleIndex();
			std::vector<std::wstring> todayCourses=schedule[dayIndex];
			*/
			
			// 绘制课程表标题
			RECT titleRect={10,10,rect.right-10,10+titlesize};
			// 使用 CreateFontW 而不是 CreateFont
			HFONT hFont=CreateFontW(titlesize,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
				DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
				CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
				VARIABLE_PITCH,L"微软雅黑");
			HFONT hOldFont=(HFONT)SelectObject(hdc,hFont);
			DrawTextW(hdc,todayCourses[0].c_str(),-1,&titleRect,DT_LEFT);
			
			// 绘制课程内容
			// 使用 CreateFontW 而不是 CreateFont
			HFONT hContentFont=CreateFontW(coursesize,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
				DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
				CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,
				VARIABLE_PITCH,L"微软雅黑");
			SelectObject(hdc,hContentFont);
			
			for (size_t i=0;i<todayCourses.size();++i){
				RECT courseRect={20,static_cast<LONG>(10+titlesize+i*coursesize),rect.right-10,static_cast<LONG>(10+titlesize+coursesize+i*coursesize)};//左上右下
				DrawTextW(hdc,todayCourses[i+1].c_str(),-1,&courseRect,DT_LEFT);
			}
			
			// 恢复原来的字体并删除创建的字体
			SelectObject(hdc,hOldFont);
			DeleteObject(hFont);
			DeleteObject(hContentFont);
			
			EndPaint(hWnd,&ps);
			break;
		}
		
		case WM_TIMER: {
			// 每天刷新一次课程表
			InvalidateRect(hWnd,NULL,TRUE);
			break;
		}
		
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	default:
		return DefWindowProcW(hWnd,message,wParam,lParam);
	}
	return 0;
}

// 注册窗口类
bool RegisterWindowClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;
	wcex.cbSize=sizeof(WNDCLASSEXW);
	wcex.style=CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc=WndProc;
	wcex.cbClsExtra=0;
	wcex.cbWndExtra=0;
	wcex.hInstance=hInstance;
	wcex.hIcon=LoadIconW(NULL,(LPCWSTR)IDI_APPLICATION);
	wcex.hCursor=LoadCursorW(NULL,(LPCWSTR)IDC_ARROW);
	wcex.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName=NULL;
	wcex.lpszClassName=L"DesktopScheduleClass";
	wcex.hIconSm=LoadIconW(NULL,(LPCWSTR)IDI_APPLICATION);
	
	return RegisterClassExW(&wcex) != 0;
}

// 获取桌面窗口句柄
HWND GetDesktopWindowHandle() {
	// 获取ProgMan窗口
	HWND hProgMan=FindWindowW(L"ProgMan",NULL);
	if (hProgMan) {
		// 查找SHELLDLL_DefView窗口
		HWND hShellDll=FindWindowExW(hProgMan,NULL,L"SHELLDLL_DefView",NULL);
		if (hShellDll) {
			// 查找SysListView32窗口（桌面图标窗口）
			return FindWindowExW(hShellDll,NULL,L"SysListView32",NULL);
		}
	}
	return NULL;
}

// 创建窗口
HWND CreateWindowInstance(HINSTANCE hInstance,int nCmdShow) {
	// 获取屏幕尺寸
	int screenWidth=GetSystemMetrics(SM_CXSCREEN);
	//int screenHeight=GetSystemMetrics(SM_CYSCREEN);
	
	// 计算窗口位置（右下角）
	int x=screenWidth-WINDOW_WIDTH-MARGIN;
	int y=MARGIN;
	
	// 获取桌面窗口句柄
	HWND hDesktop=GetDesktopWindowHandle();
	if (!hDesktop) {
		hDesktop=GetDesktopWindow();
	}
	
	HWND hWnd=CreateWindowExW(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
		L"DesktopScheduleClass",
		L"桌面课程表",
		WS_POPUP,
		x,y,
		WINDOW_WIDTH,WINDOW_HEIGHT,
		hDesktop,// 将桌面窗口作为父窗口
		NULL,
		hInstance,
		NULL
		);
	
	if (!hWnd) {
		return NULL;
	}
	
	// 设置窗口半透明
	SetLayeredWindowAttributes(hWnd,0,191,LWA_ALPHA);
	
	// 将窗口置于底部
	SetWindowPos(hWnd,HWND_BOTTOM,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	
	return hWnd;
}

// 主函数
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) {
	// 注册窗口类
	if (!RegisterWindowClass(hInstance)) {
		MessageBoxW(NULL,L"窗口类注册失败!",L"错误",MB_ICONERROR);
		return 1;
	}
	
	// 创建窗口
	hMainWnd=CreateWindowInstance(hInstance,nCmdShow);
	if (!hMainWnd) {
		MessageBoxW(NULL,L"窗口创建失败!",L"错误",MB_ICONERROR);
		return 1;
	}
	
	// 设置定时器，每小时刷新一次
	SetTimer(hMainWnd,1,60*60*1000,NULL);
	
	// 消息循环
	MSG msg;
	while (GetMessage(&msg,NULL,0,0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return (int)msg.wParam;
}
