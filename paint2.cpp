#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include<stdio.h>
#include <iomanip>
#include <tchar.h>  
#include<fstream>
#include<map>
#include<string>
#include <cmath>  
#include <vector>  
#include<iostream>
using namespace std;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

enum type {
    none,
    circle,
    rectangle,
    line,
    fre,
    multiline,
    curve,
    save,
    lin
};

enum color {
    black,
    red
};

type curtype = none;

struct info {
    type filetype = none;
    POINT pos1 = { 0,0 };
    POINT pos2 = { 0,0 };
    POINT pos3 = { 0,0 };
    POINT pos4 = { 0,0 };
    color col = black;
};

struct multilines {
    info p[100];
    int mpointer = 0;
};

LPCWSTR stringToLPCWSTR(std::string orig)
{
    size_t origsize = orig.length() + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
    mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
    return wcstring;
}

typedef struct
{
    POINT c;
    int r;
}CIRCLE;

void IntersectionOf2Circles(CIRCLE c1, CIRCLE c2, POINT& P1, POINT& P2)
{
    int a1, b1, R1, a2, b2, R2;
    a1 = c1.c.x;
    b1 = c1.c.y;
    R1 = c1.r;

    a2 = c2.c.x;
    b2 = c2.c.y;
    R2 = c2.r;
    int R1R1 = R1 * R1;
    int a1a1 = a1 * a1;
    int b1b1 = b1 * b1;
    int a2a2 = a2 * a2;
    int b2b2 = b2 * b2;
    int R2R2 = R2 * R2;
    int subs1 = a1a1 - 2 * a1 * a2 + a2a2 + b1b1 - 2 * b1 * b2 + b2b2;
    int subs2 = -R1R1 * a1 + R1R1 * a2 + R2R2 * a1 - R2R2 * a2 + a1a1 * a1 - a1a1 * a2 - a1 * a2a2 + a1 * b1b1 - 2 * a1 * b1 * b2 + a1 * b2b2 + a2a2 * a2 + a2 * b1b1 - 2 * a2 * b1 * b2 + a2 * b2b2;
    int subs3 = -R1R1 * b1 + R1R1 * b2 + R2R2 * b1 - R2R2 * b2 + a1a1 * b1 + a1a1 * b2 - 2 * a1 * a2 * b1 - 2 * a1 * a2 * b2 + a2a2 * b1 + a2a2 * b2 + b1b1 * b1 - b1b1 * b2 - b1 * b2b2 + b2b2 * b2;
    int sigma = sqrt((R1R1 + 2 * R1 * R2 + R2R2 - a1a1 + 2 * a1 * a2 - a2a2 - b1b1 + 2 * b1 * b2 - b2b2) * (-R1R1 + 2 * R1 * R2 - R2R2 + subs1));
    if (abs(subs1) > 0.0000001)//分母不为0
    {
        P1.x = (subs2 - sigma * b1 + sigma * b2) / (2 * subs1);
        P2.x = (subs2 + sigma * b1 - sigma * b2) / (2 * subs1);

        P1.y = (subs3 + sigma * a1 - sigma * a2) / (2 * subs1);
        P2.y = (subs3 - sigma * a1 + sigma * a2) / (2 * subs1);
    }

}

//求垂足
typedef struct Line {
    POINT  startPoint;
    POINT  endPoint;
}Line;

//求点到直线的距离 和 垂足
double getDistanceP2L(POINT point, Line line, POINT& p_point)
{
    double x1 = line.startPoint.x;
    double y1 = line.startPoint.y;
    double x2 = line.endPoint.x;
    double y2 = line.endPoint.y;
    double x0 = point.x;
    double y0 = point.y;

    double k = x1 == x2 ? 100000 : (y2 - y1) / (x2 - x1);//当x1=x2时，给斜率设一个较大值100000
    double a = k;
    double b = -1;
    double c = y1 - k * x1;
    //垂足
    double px = (b * b * x0 - a * b * y0 - a * c) / (a * a + b * b);
    double py = (a * a * y0 - a * b * x0 - b * c) / (a * a + b * b);
    p_point.x = px;
    p_point.y = py;
    double d = abs(a * x0 + b * y0 + c) / sqrt(a * a + b * b);
    return d;
}

//计算两个线段的交点
// 定义直线参数结构体  
struct LinePara
{
    float k;
    float b;
};

// 获取直线参数  
void getLinePara(float& x1, float& y1, float& x2, float& y2, LinePara& LP)
{
    double m = 0;
    m = x2 - x1;
    if (0 == m)
    {
        LP.k = 10000.0;
        LP.b = y1 - LP.k * x1;
    }
    else
    {
        LP.k = (y2 - y1) / (x2 - x1);
        LP.b = y1 - LP.k * x1;
    }
}

// 获取交点  
bool getCross(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3, float& x4, float& y4, POINT& pt) {
    LinePara para1, para2;
    getLinePara(x1, y1, x2, y2, para1);
    getLinePara(x3, y3, x4, y4, para2);
    if (abs(para1.k - para2.k) > 0.5)
    {
        pt.x = (para2.b - para1.b) / (para1.k - para2.k);
        pt.y = para1.k * pt.x + para1.b;
        return true;
    }
    else
    {
        return false;
    }
}
 
std::vector<POINT> findSegmentCircleIntersections(double x1, double y1, double x2, double y2, double x_start, double y_start, double x_end, double y_end) {
    std::vector<POINT> intersections; 
    double h = (x1 + x2) / 2;
    double k = (y1 + y2) / 2;
    double r = min((x2 - x1) / 2, (y2 - y1) / 2);
    double dx = x_end - x_start;
    double dy = y_end - y_start;
    double A = dx * dx + dy * dy;
    double B = 2 * (dx * (x_start - h) + dy * (y_start - k));
    double C = (x_start - h) * (x_start - h) + (y_start - k) * (y_start - k) - r * r;
    double D = B * B - 4 * A * C;

    if (D >= 0) { // 有交点  
        double t1 = (-B + sqrt(D)) / (2 * A);
        double t2 = (-B - sqrt(D)) / (2 * A);
        POINT p1, p2;
        if (0 <= t1 && t1 <= 1) {
            p1.x = x_start + t1 * dx;
            p1.y = y_start + t1 * dy;
            intersections.push_back(p1);
        }
        if (0 <= t2 && t2 <= 1) {
            p2.x = x_start + t2 * dx;
            p2.y = y_start + t2 * dy;
            intersections.push_back(p2);
        }
    }
    return intersections;
}

info info1[1000];
int pointer = 0;
int multipointer = 0;
int zpointer = 0;
bool choose = false;
bool pselect = false;
int xian = 0;
multilines multiliness[100];
int xuan[100];
int xuanpointer = 0;

//判断点是否在线段上
int checkpinl(POINT cursorPos)
{
    for (int i = 0; i < pointer; i++)
    {
        if (info1[i].filetype == line || info1[i].filetype == lin)
        {
            if (cursorPos.x - info1[i].pos1.x == 0 || info1[i].pos2.x - info1[i].pos1.x == 0) continue;
            if (abs((cursorPos.y - info1[i].pos1.y) / (cursorPos.x - info1[i].pos1.x) - (info1[i].pos2.y - info1[i].pos1.y) / (info1[i].pos2.x - info1[i].pos1.x)) <= 0.1)
            {
                if ((cursorPos.x >= info1[i].pos1.x && cursorPos.x <= info1[i].pos2.x) || (cursorPos.x <= info1[i].pos1.x && cursorPos.x >= info1[i].pos2.x))
                {
                    if ((cursorPos.y >= info1[i].pos1.y && cursorPos.y <= info1[i].pos2.y) || (cursorPos.y <= info1[i].pos1.y && cursorPos.y >= info1[i].pos2.y))
                    {
                        return i;
                    }
                }
            }
        }
    }
    return -1;
}

//判断点是否在多义线上
int checkpinmultil(POINT cursorPos)
{
    for (int i = 0; i < zpointer; i++)
    {
        int index = multiliness[i].mpointer;
        for (int j = 0; j < index; j++)
        {
            if (cursorPos.x - multiliness[i].p[j].pos1.x == 0) continue;
            if (multiliness[i].p[j].pos2.x - multiliness[i].p[j].pos1.x == 0) continue;
            if (abs((cursorPos.y - multiliness[i].p[j].pos1.y) / (cursorPos.x - multiliness[i].p[j].pos1.x) - (multiliness[i].p[j].pos2.y - multiliness[i].p[j].pos1.y) / (multiliness[i].p[j].pos2.x - multiliness[i].p[j].pos1.x)) <= 1)
            {
                if ((cursorPos.x >= multiliness[i].p[j].pos1.x && cursorPos.x <= multiliness[i].p[j].pos2.x) || (cursorPos.x <= multiliness[i].p[j].pos1.x && cursorPos.x >= multiliness[i].p[j].pos2.x))
                {
                    if ((cursorPos.y >= multiliness[i].p[j].pos1.y && cursorPos.y <= multiliness[i].p[j].pos2.y) || (cursorPos.y <= multiliness[i].p[j].pos1.y && cursorPos.y >= multiliness[i].p[j].pos2.y))
                    {
                        return i;
                    }
                }
            }
        }
    }
    return -1;
}

//计算直线与直线交点并画出
void calintersectionlal(HWND hwnd, int i, int j)
{
    POINT pt;
    float a = (float)info1[i].pos1.x;
    float b = (float)info1[i].pos1.y;
    float c = (float)info1[i].pos2.x;
    float d = (float)info1[i].pos2.y;
    float e = (float)info1[j].pos1.x;
    float f = (float)info1[j].pos1.y;
    float g = (float)info1[j].pos2.x;
    float h = (float)info1[j].pos2.y;
    getCross(a, b, c, d, e, f, g, h, pt);
    if (((pt.x > a && pt.x < c) || (pt.x < a && pt.x > c)) && ((pt.y > b && pt.y < d) || (pt.y < b && pt.x > d)))
    {
        if (((pt.x > e && pt.x < g) || (pt.x < e && pt.x > g)) && ((pt.y > f && pt.y < h) || (pt.y < f && pt.x > h)))
        {
            HDC hdc;
            hdc = GetDC(hwnd);
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
            SelectObject(hdc, hBrush);
            int radius = 5;
            Ellipse(hdc, pt.x - radius, pt.y - radius, pt.x + radius, pt.y + radius);
            string number = "(" + to_string(pt.x) + "," + to_string(pt.y) + ")";
            TextOut(hdc, pt.x, pt.y, stringToLPCWSTR(number), number.length());
            DeleteObject(hBrush);
            ReleaseDC(hwnd, hdc);
        }
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"2022112504-曾惟清";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"2022112504-曾惟清",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

BOOL fDraw = FALSE;
BOOL fLDraw = FALSE;
BOOL fLineDraw = FALSE;
POINT ptPrevious, ptnow;
int status = 1;
int status2 = 1;
int status3 = 0;
bool vertical = false;

POINT pts[] = {
            {50, 200},  // 起点（也是第一个控制点）  
            {150, 50},  // 第二个控制点  
            {250, 200}, // 第三个控制点（也是终点）  
            {250, 200}
};
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        return 0;
    }

    case WM_CREATE:
    {
        HMENU hMenu = CreateMenu();
        HMENU hSubMenu = CreatePopupMenu();
        HMENU hhSubMenu = CreatePopupMenu();
        HMENU hhhSubMenu = CreatePopupMenu();

        AppendMenu(hhSubMenu, MF_STRING, 10, L"保存");
        AppendMenu(hhSubMenu, MF_STRING, 11, L"打开");
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hhSubMenu, L"文件");

        AppendMenu(hSubMenu, MF_STRING, 1, L"圆");
        AppendMenu(hSubMenu, MF_STRING, 6, L"直线");
        AppendMenu(hSubMenu, MF_STRING, 2, L"矩形");
        AppendMenu(hSubMenu, MF_STRING, 4, L"多义线");
        AppendMenu(hSubMenu, MF_STRING, 5, L"曲线");
        AppendMenu(hSubMenu, MF_STRING, 3, L"自由绘制");
        AppendMenu(hSubMenu, MF_STRING, 7, L"清空");
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"绘制");

        AppendMenu(hhhSubMenu, MF_STRING, 22, L"选择");
        AppendMenu(hhhSubMenu, MF_STRING, 25, L"计算交点");
        AppendMenu(hhhSubMenu, MF_STRING, 20, L"计算直线与直线的交点");
        AppendMenu(hhhSubMenu, MF_STRING, 23, L"计算直线与圆形的交点");
        AppendMenu(hhhSubMenu, MF_STRING, 24, L"计算圆形与圆形的交点");
        AppendMenu(hhhSubMenu, MF_STRING, 21, L"绘制垂线");
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hhhSubMenu, L"工具");

        SetMenu(hwnd, hMenu);
        for (int i = 0; i < 100; i++)
        {
            xuan[i] = 101;
        }
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case 1:
            curtype = circle;
            choose = false;
            break;
        case 2:
            curtype = rectangle;
            choose = false;
            break;
        case 3:
            curtype = fre;
            choose = false;
            break;
        case 6:
            curtype = line;
            choose = false;
            break;
        case 4:
            curtype = multiline;
            choose = false;
            break;
        case 5:
            curtype = curve;
            choose = false;
            break;
        case 7:
        {
            hdc = GetDC(hwnd);
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, hBrush);
            SelectObject(hdc, oldBrush);
            choose = false;
            for (int i = 0; i < pointer; i++)
            {
                info1[i].filetype = none;
                info1[i].pos1 = { 0 ,0 };
                info1[i].pos2 = { 0 ,0 };
                info1[i].pos3 = { 0 ,0 };
                info1[i].pos4 = { 0 ,0 };
            }
            break;
            DeleteObject(hBrush);
            DeleteObject(oldBrush);
        }
        case 10:
        {
            OPENFILENAME ofn{};
            TCHAR szFile[260]{};       //文件名缓冲区
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL; //设置对话框拥有者句柄
            ofn.lpstrFile = szFile; //设置文件名缓冲区
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile); //设置所选文件路径缓冲区最大长度
            ofn.lpstrFilter = L"全部文件\0*.*\0文本文件\0*.txt\0";  //指定文件类型
            ofn.nFilterIndex = 1;  //文件类型选中的索引 从1开始
            ofn.lpstrFileTitle = NULL;  //选中的文件名（不包含路径，包含扩展名）
            ofn.nMaxFileTitle = 0;  //选中的文件名缓冲区最大长度
            ofn.lpstrInitialDir = NULL;  //初始路径
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; //用于初始化对话框的标志位 
            if (GetSaveFileName(&ofn) == TRUE)
                std::wcout << ofn.lpstrFile << std::endl;
            std::ofstream outFile(ofn.lpstrFile, std::ios::trunc);
            for (int i = 0; i < pointer; i++)
            {
                string tname;
                switch (info1[i].filetype)
                {
                case circle:tname = "circle"; break;
                case rectangle:tname = "rectangle"; break;
                case line:tname = "lin"; break;
                case curve:tname = "curve"; break;
                }
                outFile << tname << std::endl;
                outFile << info1[i].pos1.x << std::endl;
                outFile << info1[i].pos1.y << std::endl;
                outFile << info1[i].pos2.x << std::endl;
                outFile << info1[i].pos2.y << std::endl;
                outFile << info1[i].pos3.x << std::endl;
                outFile << info1[i].pos3.y << std::endl;
                outFile << info1[i].pos4.x << std::endl;
                outFile << info1[i].pos4.y << std::endl;
            }
            for (int i = 0; i < zpointer; i++)
            {
                outFile << "multiline" << std::endl;
                for (int j = 0; j < multiliness[i].mpointer; j++)
                {
                    outFile << multiliness[i].p[j].pos1.x << std::endl;
                    outFile << multiliness[i].p[j].pos1.y << std::endl;
                    outFile << multiliness[i].p[j].pos2.x << std::endl;
                    outFile << multiliness[i].p[j].pos2.y << std::endl;
                    outFile << multiliness[i].p[j].pos3.x << std::endl;
                    outFile << multiliness[i].p[j].pos3.y << std::endl;
                    outFile << multiliness[i].p[j].pos4.x << std::endl;
                    outFile << multiliness[i].p[j].pos4.y << std::endl;
                }
            }
            outFile.close();
            //MessageBox(hwnd, L"  数据已保存", L"提示", MB_OK);
            break;
        }
        case 11:
        {
            hdc = GetDC(hwnd);
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, hBrush);
            SelectObject(hdc, oldBrush);
            choose = false;
            OPENFILENAME ofn{};
            TCHAR szFile[260]{};       //文件名缓冲区
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL; //设置对话框拥有者句柄
            ofn.lpstrFile = szFile; //设置文件名缓冲区
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(szFile); //设置所选文件路径缓冲区最大长度
            ofn.lpstrFilter = L"全部文件\0*.*\0文本文件\0*.txt\0";  //指定文件类型
            ofn.nFilterIndex = 1;  //文件类型选中的索引 从1开始
            ofn.lpstrFileTitle = NULL;  //选中的文件名（不包含路径，包含扩展名）
            ofn.nMaxFileTitle = 0;  //选中的文件名缓冲区最大长度
            ofn.lpstrInitialDir = NULL;  //初始路径
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; //用于初始化对话框的标志位 
            if (GetOpenFileName(&ofn) == TRUE)
                std::wcout << ofn.lpstrFile << std::endl;
            std::ifstream inFile(ofn.lpstrFile); // 打开文件用于读取  
            int ci;
            std::string line;
            for (int i = 0; i < pointer; i++)
            {
                info1[i].filetype = none;
                info1[i].pos1 = { 0 ,0 };
                info1[i].pos2 = { 0 ,0 };
                info1[i].pos3 = { 0 ,0 };
                info1[i].pos4 = { 0 ,0 };
            }
            pointer = 0;
            zpointer = 0;
            for (int i = 0; i < 100; i++)
            {
                multiliness[i].mpointer = 0;
                for (int j = 0; j < 100; j++)
                {
                    multiliness[i].p[j].pos1 = { 0,0 };
                    multiliness[i].p[j].pos2 = { 0,0 };
                    multiliness[i].p[j].pos3 = { 0,0 };
                    multiliness[i].p[j].pos4 = { 0,0 };
                }
            }
            // 使用循环读取文件的每一行，直到文件结束  
            while (std::getline(inFile, line))
            {
                if (line == "") break;
                if (line == "curve")
                {
                    ci = 4;
                    info1[pointer].filetype = curve;
                }
                else ci = 2;
                if (line == "lin") info1[pointer].filetype = lin;
                if (line == "circle") info1[pointer].filetype = circle;
                if (line == "rectangle") info1[pointer].filetype = rectangle;
                if (line == "multiline")
                {
                    while (getline(inFile, line))
                    {
                        if (line == "") break;
                        int index = multiliness[zpointer].mpointer;
                        multiliness[zpointer].p[index].pos1.x = std::stoi(line);
                        getline(inFile, line);
                        multiliness[zpointer].p[index].pos1.y = std::stoi(line);
                        getline(inFile, line);
                        multiliness[zpointer].p[index].pos2.x = std::stoi(line);
                        getline(inFile, line);
                        multiliness[zpointer].p[index].pos2.y = std::stoi(line);
                        MoveToEx(hdc, multiliness[zpointer].p[index].pos1.x, multiliness[zpointer].p[index].pos1.y, NULL);
                        LineTo(hdc, multiliness[zpointer].p[index].pos2.x, multiliness[zpointer].p[index].pos2.y);
                        multiliness[zpointer].mpointer++;
                    }
                    zpointer++;
                    continue;
                }
                if (ci == 2)
                {
                    std::getline(inFile, line);
                    info1[pointer].pos1.x = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos1.y = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos2.x = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos2.y = std::stoi(line);
                    std::getline(inFile, line);
                    std::getline(inFile, line);
                    std::getline(inFile, line);
                    std::getline(inFile, line);
                    if (info1[pointer].filetype == lin)
                    {
                        MoveToEx(hdc, info1[pointer].pos1.x, info1[pointer].pos1.y, NULL);
                        LineTo(hdc, info1[pointer].pos2.x, info1[pointer].pos2.y);
                    }
                    if (info1[pointer].filetype == circle)
                    {
                        HBRUSH hBrush;
                        hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//空心，画刷取空
                        SelectObject(hdc, hBrush);
                        Ellipse(hdc, info1[pointer].pos1.x, info1[pointer].pos1.y, info1[pointer].pos2.x, info1[pointer].pos2.y);
                    }
                    if (info1[pointer].filetype == rectangle)
                    {
                        HBRUSH hBrush;
                        hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//空心，画刷取空
                        SelectObject(hdc, hBrush);
                        Rectangle(hdc, info1[pointer].pos1.x, info1[pointer].pos1.y, info1[pointer].pos2.x, info1[pointer].pos2.y);
                    }
                }
                else if (ci == 4)
                {
                    std::getline(inFile, line);
                    info1[pointer].pos1.x = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos1.y = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos2.x = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos2.y = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos3.x = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos3.y = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos4.x = std::stoi(line);
                    std::getline(inFile, line);
                    info1[pointer].pos4.y = std::stoi(line);
                    POINT pts2[] =
                    {
                    {info1[pointer].pos1.x, info1[pointer].pos1.y},  // 起点（也是第一个控制点）  
                    {info1[pointer].pos2.x, info1[pointer].pos2.y},  // 第二个控制点  
                    {info1[pointer].pos3.x, info1[pointer].pos3.y}, // 第三个控制点（也是终点）  
                    {info1[pointer].pos4.x, info1[pointer].pos4.y}
                    };
                    PolyBezier(hdc, pts2, 4);
                }
                pointer++;
            }
            // 关闭文件  
            inFile.close();
            DeleteObject(hBrush);
            DeleteObject(oldBrush);
            ReleaseDC(hwnd, hdc);
            break;
        }
        case 22:
        {
            choose = true;
            break;
        }
        //计算直线与直线的交点
        case 20:
        {
            for (int i = 0; i < pointer; i++)
            {
                if (info1[i].filetype == line || info1[i].filetype == lin)
                {
                    for (int j = i + 1; j < pointer; j++)
                    {
                        if (info1[j].filetype == line || info1[j].filetype == lin)
                        {
                            calintersectionlal(hwnd, i, j);
                        }
                    }
                }
            }
            break;
        }
        //计算直线与圆的交点
        case 23:
        {
            for (int i = 0; i < pointer; i++)
            {
                if (info1[i].filetype == line || info1[i].filetype == lin)
                {
                    for (int j = 0; j < pointer; j++)
                    {
                        if (info1[j].filetype == circle)
                        {
                            std::vector<POINT> intersections = findSegmentCircleIntersections(info1[j].pos1.x, info1[j].pos1.y, info1[j].pos2.x, info1[j].pos2.y, info1[i].pos1.x, info1[i].pos1.y, info1[i].pos2.x, info1[i].pos2.y);
                            for (const auto& point : intersections)
                            {
                                hdc = GetDC(hwnd);
                                // 选择一个实心刷子  
                                HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
                                SelectObject(hdc, hBrush);
                                int radius = 5;
                                Ellipse(hdc, point.x - radius, point.y - radius, point.x + radius, point.y + radius);
                                string number = "(" + to_string(point.x) + "," + to_string(point.y) + ")";
                                TextOut(hdc, point.x, point.y, stringToLPCWSTR(number), number.length());
                                DeleteObject(hBrush);
                                ReleaseDC(hwnd, hdc);
                            }
                        }
                    }
                }
            }
            break;
        }
        //圆形与圆形的交点
        case 24:
        {
            for (int i = 0; i < pointer; i++)
            {
                if (info1[i].filetype == circle)
                {
                    for (int j = i + 1; j < pointer; j++)
                    {
                        if (info1[j].filetype == circle)
                        {
                            hdc = GetDC(hwnd);
                            POINT P1;
                            POINT P2;
                            CIRCLE c1;
                            CIRCLE c2;
                            c1.c = { (info1[i].pos1.x + info1[i].pos2.x) / 2,(info1[i].pos1.y + info1[i].pos2.y) / 2 };
                            c1.r = (info1[i].pos1.x - info1[i].pos2.x) / 2;
                            c2.c = { (info1[j].pos1.x + info1[j].pos2.x) / 2,(info1[j].pos1.y + info1[j].pos2.y) / 2 };
                            c2.r = (info1[j].pos1.x - info1[j].pos2.x) / 2;
                            IntersectionOf2Circles(c1, c2, P1, P2);
                            // 选择一个实心刷子  
                            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
                            SelectObject(hdc, hBrush);
                            int radius = 5;
                            Ellipse(hdc, P1.x - radius, P1.y - radius, P1.x + radius, P1.y + radius);
                            Ellipse(hdc, P2.x - radius, P2.y - radius, P2.x + radius, P2.y + radius);
                            string number = "(" + to_string(P1.x) + "," + to_string(P1.y) + ")";
                            TextOut(hdc, P1.x, P1.y, stringToLPCWSTR(number), number.length());
                            string number2 = "(" + to_string(P2.x) + "," + to_string(P2.y) + ")";
                            TextOut(hdc, P2.x, P2.y, stringToLPCWSTR(number2), number.length());
                            DeleteObject(hBrush);
                            ReleaseDC(hwnd, hdc);
                        }
                    }
                }
            }
            break;
        }
        case 21:
        {
            vertical = true;
            break;
        }
        case 25:
        {
            for (int i = 0; i < xuanpointer; i++)
            {
                for (int j = 0; j < xuanpointer; j++)
                {
                    if (info1[xuan[i]].filetype == line || info1[xuan[i]].filetype == lin)
                    {
                        //直线与直线
                        if (info1[xuan[j]].filetype == line || info1[xuan[j]].filetype == lin)
                        {
                            calintersectionlal(hwnd, xuan[i], xuan[j]);
                        }
                        //直线与圆形
                        if (info1[xuan[j]].filetype == circle)
                        {
                            std::vector<POINT> intersections = findSegmentCircleIntersections(info1[xuan[j]].pos1.x, info1[xuan[j]].pos1.y, info1[xuan[j]].pos2.x, info1[xuan[j]].pos2.y, info1[xuan[i]].pos1.x, info1[xuan[i]].pos1.y, info1[xuan[i]].pos2.x, info1[xuan[i]].pos2.y);
                            for (const auto& point : intersections)
                            {
                                hdc = GetDC(hwnd);
                                // 选择一个实心刷子  
                                HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
                                SelectObject(hdc, hBrush);
                                int radius = 5;
                                Ellipse(hdc, point.x - radius, point.y - radius, point.x + radius, point.y + radius);
                                string number = "(" + to_string(point.x) + "," + to_string(point.y) + ")";
                                TextOut(hdc, point.x, point.y, stringToLPCWSTR(number), number.length());
                                DeleteObject(hBrush);
                                ReleaseDC(hwnd, hdc);
                            }
                        }
                    }
                    if (info1[xuan[i]].filetype == circle)
                    {
                        //圆形与圆形
                        if (info1[xuan[j]].filetype == circle)
                        {
                            hdc = GetDC(hwnd);
                            POINT P1;
                            POINT P2;
                            CIRCLE c1;
                            CIRCLE c2;
                            c1.c = { (info1[xuan[i]].pos1.x + info1[xuan[i]].pos2.x) / 2,(info1[xuan[i]].pos1.y + info1[xuan[i]].pos2.y) / 2 };
                            c1.r = (info1[xuan[i]].pos1.x - info1[xuan[i]].pos2.x) / 2;
                            c2.c = { (info1[xuan[j]].pos1.x + info1[xuan[j]].pos2.x) / 2,(info1[xuan[j]].pos1.y + info1[xuan[j]].pos2.y) / 2 };
                            c2.r = (info1[xuan[j]].pos1.x - info1[xuan[j]].pos2.x) / 2;
                            IntersectionOf2Circles(c1, c2, P1, P2);
                            // 选择一个实心刷子  
                            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
                            SelectObject(hdc, hBrush);
                            int radius = 5;
                            Ellipse(hdc, P1.x - radius, P1.y - radius, P1.x + radius, P1.y + radius);
                            Ellipse(hdc, P2.x - radius, P2.y - radius, P2.x + radius, P2.y + radius);
                            string number = "(" + to_string(P1.x) + "," + to_string(P1.y) + ")";
                            TextOut(hdc, P1.x, P1.y, stringToLPCWSTR(number), number.length());
                            string number2 = "(" + to_string(P2.x) + "," + to_string(P2.y) + ")";
                            TextOut(hdc, P2.x, P2.y, stringToLPCWSTR(number2), number.length());
                            DeleteObject(hBrush);
                            ReleaseDC(hwnd, hdc);
                        }
                    }
                }
            }
        }
        return 0;
        }
    }
    case WM_LBUTTONDOWN:
    {
        POINT cursorPos = { LOWORD(lParam),HIWORD(lParam) };
        if (vertical == true)
        {
            hdc = GetDC(hwnd);
            if (cursorPos.x == 0 && cursorPos.y == 0) break;
            int t = checkpinl(cursorPos);
            if (t == -1) break;
            HPEN hPen;
            hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
            SelectObject(hdc, hPen);
            MoveToEx(hdc, info1[t].pos1.x, info1[t].pos1.y, NULL);
            LineTo(hdc, info1[t].pos2.x, info1[t].pos2.y);
            xian = t;
            pselect = true;
            vertical = false;
            DeleteObject(hPen);
            ReleaseDC(hwnd, hdc);
            break;
        }
        if (pselect == true)
        {
            hdc = GetDC(hwnd);
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
            SelectObject(hdc, hBrush);
            int radius = 5;
            Line line;
            POINT t_point;
            line.startPoint = info1[xian].pos1;
            line.endPoint = info1[xian].pos2;
            getDistanceP2L(cursorPos, line, t_point);
            MoveToEx(hdc, cursorPos.x, cursorPos.y, NULL);
            LineTo(hdc, t_point.x, t_point.y);
            Ellipse(hdc, t_point.x - radius, t_point.y - radius, t_point.x + radius, t_point.y + radius);
            string number = "(" + to_string(t_point.x) + "," + to_string(t_point.y) + ")";
            TextOut(hdc, t_point.x, t_point.y, stringToLPCWSTR(number), number.length());
            pselect = false;
            DeleteObject(hBrush);
            ReleaseDC(hwnd, hdc);
            curtype = none;
        }
        if (choose == true)
        {
            for (int i = 0; i < pointer; i++)
            {
                switch (info1[i].filetype)
                {
                case rectangle:
                    if (cursorPos.x >= info1[i].pos1.x && cursorPos.x <= info1[i].pos2.x && cursorPos.y >= info1[i].pos1.y && cursorPos.y <= info1[i].pos2.y)
                    {
                        hdc = GetDC(hwnd);
                        HPEN hPen;
                        hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                        if (info1[i].col == black)
                        {
                            hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                            info1[i].col = red;
                        }
                        else if (info1[i].col == red)
                        {
                            hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                            info1[i].col = black;
                        }
                        SelectObject(hdc, hPen);
                        HBRUSH hBrush;
                        hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//空心，画刷取空
                        SelectObject(hdc, hBrush);
                        Rectangle(hdc, info1[i].pos1.x, info1[i].pos1.y, info1[i].pos2.x, info1[i].pos2.y);
                        DeleteObject(hPen);
                        DeleteObject(hBrush);
                        ReleaseDC(hwnd, hdc);
                    }
                    break;
                case line:
                {
                    hdc = GetDC(hwnd);
                    int t = checkpinl(cursorPos);
                    if (t == -1) break;
                    HPEN hPen;
                    hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                    SelectObject(hdc, hPen);
                    MoveToEx(hdc, info1[t].pos1.x, info1[t].pos1.y, NULL);
                    LineTo(hdc, info1[t].pos2.x, info1[t].pos2.y);
                    DeleteObject(hPen);
                    ReleaseDC(hwnd, hdc);
                    int exist = 0;
                    for (int k = 0; k < 100; k++)
                    {
                        if (xuan[k] == t) exist = 1;
                    }
                    if (exist == 0)
                    {
                        xuan[xuanpointer] = t;
                        xuanpointer++;
                    }
                    break;
                }
                case circle:
                    POINT heart = { (info1[i].pos1.x + info1[i].pos2.x) / 2,(info1[i].pos1.y + info1[i].pos2.y) / 2 };
                    int longr = abs(info1[i].pos2.x - info1[i].pos1.x) / 2;
                    int shortr = abs(info1[i].pos2.y - info1[i].pos1.y) / 2;
                    if ((pow(cursorPos.x - heart.x, 2) / (longr * longr)) + (pow(cursorPos.y - heart.y, 2) / (shortr * shortr)) <= 1)
                    {
                        hdc = GetDC(hwnd);
                        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                        SetDCBrushColor(hdc, RGB(255, 0, 0));
                        // 选择一个实心刷子  
                        HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
                        SelectObject(hdc, hBrush);
                        int radius = 5;
                        Ellipse(hdc, heart.x - radius, heart.y - radius, heart.x + radius, heart.y + radius);

                        string number = "(" + to_string(heart.x) + "," + to_string(heart.y) + ")";
                        TextOut(hdc, heart.x, heart.y, stringToLPCWSTR(number), number.length());
                        if (info1[i].col == black)
                        {
                            hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                            info1[i].col = red;
                        }
                        else if (info1[i].col == red)
                        {
                            hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                            info1[i].col = black;
                        }
                        SelectObject(hdc, hPen);
                        hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//空心，画刷取空
                        SelectObject(hdc, hBrush);
                        Ellipse(hdc, info1[i].pos1.x, info1[i].pos1.y, info1[i].pos2.x, info1[i].pos2.y);
                        DeleteObject(hPen);
                        DeleteObject(hBrush);
                        ReleaseDC(hwnd, hdc);
                        xuan[xuanpointer] = i;
                        xuanpointer++;
                    }
                    break;
                }
            }
            for (int i = 0; i < zpointer; i++)
            {
                int index = multiliness[i].mpointer;
                for (int j = 0; j < index; j++)
                {
                    int t = checkpinmultil(cursorPos);
                    if (t == -1) continue;
                    int index2 = multiliness[t].mpointer;
                    for (int k = 0; k < index2; k++)
                    {
                        HPEN hPen;
                        hdc = GetDC(hwnd);
                        hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                        if (multiliness[t].p[k].col == black)
                        {
                            hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
                            multiliness[t].p[k].col = red;
                        }
                        else if (multiliness[t].p[k].col == red)
                        {
                            hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                            multiliness[t].p[k].col = black;
                        }
                        SelectObject(hdc, hPen);
                        MoveToEx(hdc, multiliness[t].p[k].pos1.x, multiliness[t].p[k].pos1.y, NULL);
                        LineTo(hdc, multiliness[t].p[k].pos2.x, multiliness[t].p[k].pos2.y);
                        DeleteObject(hPen);
                        ReleaseDC(hwnd, hdc);
                    }
                    break;
                }
            }
            break;
        }
        if (curtype == fre)
        {
            ptPrevious.x = LOWORD(lParam);
            ptPrevious.y = HIWORD(lParam);
            if (ptPrevious.x == 0 && ptPrevious.y == 0) break;
            fDraw = TRUE;
            break;
        }
        if (curtype == circle)
        {
            fLDraw = TRUE;
            ptPrevious.x = LOWORD(lParam);
            ptPrevious.y = HIWORD(lParam);
            break;
        }
        if ((curtype == line || curtype == multiline) && status == 2)
        {
            hdc = GetDC(hwnd);
            fLineDraw = TRUE;
            ptnow.x = LOWORD(lParam);
            ptnow.y = HIWORD(lParam);
            if (ptnow.x == 0 && ptnow.y == 0)
            {
                status = 1;
                break;
            }
            MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
            LineTo(hdc, ptnow.x, ptnow.y);
            if (curtype == line)
            {
                info1[pointer].filetype = line;
                info1[pointer].pos1 = { ptPrevious.x,ptPrevious.y };
                info1[pointer].pos2 = { ptnow.x,ptnow.y };
                pointer++;
            }
            if (curtype == multiline)
            {
                int index = multiliness[zpointer].mpointer;
                multiliness[zpointer].p[index].pos1 = { ptPrevious.x, ptPrevious.y };
                multiliness[zpointer].p[index].pos2 = { ptnow.x, ptnow.y };
                multiliness[zpointer].mpointer++;
            }
            status = 1;
            if (curtype == line) break;
            ReleaseDC(hwnd, hdc);
        }
        if ((curtype == line || curtype == multiline) && status == 1)
        {
            fLineDraw = TRUE;
            ptPrevious.x = LOWORD(lParam);
            ptPrevious.y = HIWORD(lParam);
            if (ptPrevious.x == 0 && ptPrevious.y == 0) break;
            status = 2;
            break;
        }
        if (curtype == rectangle && status2 == 2)
        {
            hdc = GetDC(hwnd);
            ptnow.x = LOWORD(lParam);
            ptnow.y = HIWORD(lParam);
            HBRUSH hBrush;
            hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//空心，画刷取空
            SelectObject(hdc, hBrush);
            Rectangle(hdc, ptPrevious.x, ptPrevious.y, ptnow.x, ptnow.y);
            info1[pointer].filetype = rectangle;
            info1[pointer].pos1 = { ptPrevious.x,ptPrevious.y };
            info1[pointer].pos2 = { ptnow.x,ptnow.y };
            pointer++;
            status2 = 1;
            DeleteObject(hBrush);
            ReleaseDC(hwnd, hdc);
            break;
        }
        if (curtype == rectangle && status2 == 1)
        {
            ptPrevious.x = LOWORD(lParam);
            ptPrevious.y = HIWORD(lParam);
            if (ptPrevious.x == 0 && ptPrevious.y == 0) break;
            status2 = 2;
            break;
        }
        if (curtype == curve)
        {
            hdc = GetDC(hwnd);
            pts[status3] = { LOWORD(lParam) , HIWORD(lParam) };
            if (pts[status3].x == 0 && pts[status3].y == 0) break;
            status3++;
            if (status3 == 4)
            {
                PolyBezier(hdc, pts, 4);
                info1[pointer].filetype = curve;
                info1[pointer].pos1 = { pts[0].x,pts[0].y };
                info1[pointer].pos2 = { pts[1].x,pts[1].y };
                info1[pointer].pos3 = { pts[2].x,pts[2].y };
                info1[pointer].pos4 = { pts[3].x,pts[3].y };
                pointer++;
                status3 = 0;
            }
            break;
        }
        return 0;
    }

    case WM_LBUTTONUP:
    {
        if (fDraw)
        {
            hdc = GetDC(hwnd);
            MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
            LineTo(hdc, LOWORD(lParam), HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
        }
        if (fLDraw)
        {
            hdc = GetDC(hwnd);
            MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
            ptnow.x = LOWORD(lParam);
            ptnow.y = HIWORD(lParam);
            ptnow.y = ptPrevious.y + (ptnow.x - ptPrevious.x);
            HPEN hPen;
            hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));//black
            SelectObject(hdc, hPen);
            HBRUSH hBrush;
            hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//空心，画刷取空
            SelectObject(hdc, hBrush);
            Ellipse(hdc, ptPrevious.x, ptPrevious.y, ptnow.x, ptnow.y);
            info1[pointer].pos1 = { ptPrevious.x,ptPrevious.y };
            info1[pointer].pos2 = { ptnow.x,ptnow.y };
            info1[pointer].filetype = circle;
            pointer++;
            ReleaseDC(hwnd, hdc);
            DeleteObject(hPen);
            DeleteObject(hBrush);
        }
        fDraw = FALSE;
        fLDraw = FALSE;
        fLineDraw = FALSE;
        return 0;
    }

    case WM_RBUTTONDOWN:
        if (curtype == multiline)
        {
            zpointer++;
            status = 1;
        }

    case WM_MOUSEMOVE:
        if (fDraw)
        {
            hdc = GetDC(hwnd);
            MoveToEx(hdc, ptPrevious.x, ptPrevious.y, NULL);
            LineTo(hdc, ptPrevious.x = LOWORD(lParam),
                ptPrevious.y = HIWORD(lParam));
            ReleaseDC(hwnd, hdc);
        }
        if (choose == true)
        {
            int flag = 0;
            POINT cursorPos = { LOWORD(lParam),HIWORD(lParam) };
            if (checkpinl(cursorPos) != -1)
            {
                LPCTSTR cursor = IDC_HAND;
                HCURSOR hCursor = LoadCursor(NULL, cursor);
                SetCursor(hCursor);
                flag = 1;
            }
            if (checkpinmultil(cursorPos) != -1)
            {
                LPCTSTR cursor = IDC_PIN;
                HCURSOR hCursor = LoadCursor(NULL, cursor);
                SetCursor(hCursor);
                flag = 1;
            }
            for (int i = 0; i < pointer; i++)
            {
                if (info1[i].filetype == circle)
                {
                    POINT heart = { (info1[i].pos1.x + info1[i].pos2.x) / 2,(info1[i].pos1.y + info1[i].pos2.y) / 2 };
                    int longr = abs(info1[i].pos2.x - info1[i].pos1.x) / 2;
                    int shortr = abs(info1[i].pos2.y - info1[i].pos1.y) / 2;
                    if ((pow(cursorPos.x - heart.x, 2) / (longr * longr)) + (pow(cursorPos.y - heart.y, 2) / (shortr * shortr)) <= 1)
                    {
                        LPCTSTR cursor = IDC_PERSON;
                        HCURSOR hCursor = LoadCursor(NULL, cursor);
                        SetCursor(hCursor);
                        flag = 1;
                    }
                }
                if (info1[i].filetype == rectangle)
                {
                    if (cursorPos.x >= info1[i].pos1.x && cursorPos.x <= info1[i].pos2.x && cursorPos.y >= info1[i].pos1.y && cursorPos.y <= info1[i].pos2.y)
                    {
                        LPCTSTR cursor = IDC_NO;
                        HCURSOR hCursor = LoadCursor(NULL, cursor);
                        SetCursor(hCursor);
                        flag = 1;
                    }
                }
            }
            if (flag == 0)
            {
                LPCTSTR cursor = IDC_ARROW;
                HCURSOR hCursor = LoadCursor(NULL, cursor);
                SetCursor(hCursor);
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}