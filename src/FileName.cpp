#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <graphics.h>
#include <conio.h>
#include <mysql.h>
using namespace std;
#define APP_ICON "./app.ico"
IMAGE image;
const char* host = "127.0.0.1";
const char* user = "root";
const char* pw = "666666";
const char* database_name = "data";
const int port = 3306;
const char* table = "members";

struct Member {
    int id;
    std::string name;
    std::string type;
    double consumption;
    int points;

    Member() : id(0), name(""), type("Silver"), consumption(0.0), points(0) {}

    Member(int mid, std::string mname, std::string mtype, double mconsumption)
        : id(mid), name(mname), type(mtype), consumption(mconsumption) {
        points = calculatePoints(mconsumption);
    }

    int calculatePoints(double consumptionAmount) {
        return static_cast<int>(std::ceil(consumptionAmount) * 10);
    }
};

std::vector<Member> members;

bool compareMembers(const Member& a, const Member& b) {
    return a.id < b.id;
}

bool isSixDigitNumber(int id) {
    return id >= 100000 && id <= 999999;
}

bool isButtonClicked(int x1, int y1, int x2, int y2, MOUSEMSG msg) {
    return msg.x > x1 && msg.x < x2 && msg.y > y1 && msg.y < y2 && msg.uMsg == WM_LBUTTONDOWN;
}

bool isButtonHovered(int x1, int y1, int x2, int y2, MOUSEMSG msg) {
    return msg.x > x1 && msg.x < x2 && msg.y > y1 && msg.y < y2;
}

void drawButton(int x1, int y1, int x2, int y2, const char* label, bool isHovered) {
    if (isHovered) {
        setfillcolor(LIGHTGRAY);
        bar(x1, y1, x2, y2);
    }
    else {
        setfillcolor(WHITE);
        bar(x1, y1, x2, y2);
    }
    setlinecolor(BLACK);
    rectangle(x1, y1, x2, y2);

    settextcolor(BLACK);
    int textWidth = textwidth(label);
    int textHeight = textheight(label);
    setbkmode(TRANSPARENT);
    outtextxy(x1 + (x2 - x1 - textWidth) / 2, y1 + (y2 - y1 - textHeight) / 2, label);
}

void drawInputBox(int left, int top, int width, int height, const char* content) {
    setfillstyle(SOLID_FILL, WHITE);
    bar(left, top, left + width, top + height);
    rectangle(left, top, left + width, top + height);
    outtextxy(left + 5, top + 5, content);
}

void updateMemberType(Member& member) {
    if (member.consumption >= 400) {
        member.type = "Gold";
    }
    else {
        member.type = "Silver";
    }
}

void addMember() {
    cleardevice();
    putimage(0, 0, &image);
    int inputBoxWidth = 200;
    int inputBoxHeight = 30;
    int startX = 300;
    int idY = 100, nameY = 200, typeY = 300, consumptionY = 400;
    int buttonWidth = 100, buttonHeight = 50;
    int registerX = startX + (inputBoxWidth - buttonWidth) / 2;
    int registerY = consumptionY + inputBoxHeight + 50;

    std::string idInput = "", nameInput = "", typeInput = "";
    double consumptionInput = 0.0;

    bool idBoxActive = false, nameBoxActive = false, typeBoxActive = false, consumptionBoxActive = false;

    while (true) {
        BeginBatchDraw();
        cleardevice();
        putimage(0, 0, &image);

        settextstyle(50, 0, "微软雅黑");
        outtextxy(220, 40, "简单会员管理系统");
        settextstyle(20, 0, "Arial");
        outtextxy(startX - 40, idY + 25, "会员ID:");
        outtextxy(startX - 40, nameY + 25, "会员姓名:");
        outtextxy(startX - 40, typeY + 25, "会员类型:");
        outtextxy(startX - 40, consumptionY + 25, "消费金额:");

        drawInputBox(startX + 60, idY + 20, inputBoxWidth, inputBoxHeight, idInput.c_str());
        drawInputBox(startX + 60, nameY + 20, inputBoxWidth, inputBoxHeight, nameInput.c_str());
        drawInputBox(startX + 60, typeY + 20, inputBoxWidth, inputBoxHeight, typeInput.c_str());

        char consumptionStr[50];
        sprintf_s(consumptionStr, "%.2f", consumptionInput);
        drawInputBox(startX + 60, consumptionY + 20, inputBoxWidth, inputBoxHeight, consumptionStr);

        MOUSEMSG msg = GetMouseMsg();
        bool registerHovered = isButtonHovered(registerX, registerY, registerX + buttonWidth, registerY + buttonHeight, msg);
        bool returnHovered = isButtonHovered(650, 500, 750, 550, msg);

        drawButton(registerX + 10, registerY, registerX + buttonWidth, registerY + buttonHeight, "注册", registerHovered);
        drawButton(650, 500, 750, 550, "返回", returnHovered);

        if (msg.uMsg == WM_LBUTTONDOWN) {
            char temp[50] = "";

            if (msg.x >= startX && msg.x <= startX + inputBoxWidth) {
                if (msg.y >= idY && msg.y <= idY + inputBoxHeight) {
                    InputBox(temp, 7, "请输入会员 ID（6 位数字）：");
                    if (strlen(temp) > 0 && isSixDigitNumber(atoi(temp))) {
                        idInput = temp;
                        idBoxActive = true;
                    }
                    else {
                        MessageBox(GetHWnd(), "会员 ID 必须是 6 位数字！", "错误", MB_OK);
                    }
                }
                else if (msg.y >= nameY && msg.y <= nameY + inputBoxHeight) {
                    InputBox(temp, 50, "请输入会员姓名：");
                    if (strlen(temp) > 0) {
                        nameInput = temp;
                        nameBoxActive = true;
                    }
                    else {
                        MessageBox(GetHWnd(), "姓名不能为空！", "错误", MB_OK);
                    }
                }
                else if (msg.y >= typeY && msg.y <= typeY + inputBoxHeight) {
                    InputBox(temp, 10, "请输入会员类型（Gold/Silver）：");
                    if (std::string(temp) == "Gold" || std::string(temp) == "Silver") {
                        typeInput = temp;
                        typeBoxActive = true;
                    }
                    else {
                        MessageBox(GetHWnd(), "会员类型无效，请输入 Gold 或 Silver！", "错误", MB_OK);
                    }
                }
                else if (msg.y >= consumptionY && msg.y <= consumptionY + inputBoxHeight) {
                    InputBox(temp, 50, "请输入消费金额");
                    consumptionInput = atof(temp);
                    consumptionBoxActive = true;
                }
            }

            if (isButtonClicked(registerX, registerY, registerX + buttonWidth, registerY + buttonHeight, msg)) {
                if (idInput.empty() || nameInput.empty() || typeInput.empty()) {
                    MessageBox(GetHWnd(), "所有字段不能为空！", "错误", MB_OK);
                }
                else {
                    Member newMember(atoi(idInput.c_str()), nameInput, typeInput, consumptionInput);
                    updateMemberType(newMember);
                    members.push_back(newMember);
                    MessageBox(GetHWnd(), "会员注册成功！", "成功", MB_OK);
                    break;
                }
            }

            if (isButtonClicked(650, 500, 750, 550, msg)) {
                break;
            }
        }

        FlushBatchDraw();
    }
    EndBatchDraw();
}

int main() {
    initgraph(800, 600);
    loadimage(&image, "image.png", 800, 600);
    HWND hwnd = GetHWnd();
    SetWindowText(hwnd, "简单会员管理系统");
    HICON hIcon = (HICON)LoadImage(NULL, APP_ICON, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

    while (true) {
        BeginBatchDraw();
        cleardevice();
        putimage(0, 0, &image);

        settextstyle(50, 0, "微软雅黑");
        outtextxy(220, 40, "简单会员管理系统");

        MOUSEMSG msg = GetMouseMsg();
        bool addHovered = isButtonHovered(350, 200, 450, 250, msg);
        bool viewHovered = isButtonHovered(350, 300, 450, 350, msg);
        bool exitHovered = isButtonHovered(350, 400, 450, 450, msg);

        drawButton(350, 200, 450, 250, "添加会员", addHovered);
        drawButton(350, 300, 450, 350, "查看会员", viewHovered);
        drawButton(350, 400, 450, 450, "退出系统", exitHovered);

        if (msg.uMsg == WM_LBUTTONDOWN) {
            if (isButtonClicked(350, 200, 450, 250, msg)) {
                addMember();
            }
            else if (isButtonClicked(350, 300, 450, 350, msg)) {
                // 查看会员功能
            }
            else if (isButtonClicked(350, 400, 450, 450, msg)) {
                if (MessageBox(hwnd, "确定要退出系统吗？", "确认", MB_OKCANCEL) == IDOK) {
                    break;
                }
            }
        }

        FlushBatchDraw();
    }

    EndBatchDraw();
    closegraph();
    return 0;
}
