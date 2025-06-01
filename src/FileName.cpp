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

    // 默认构造函数
    Member() : id(0), name(""), type("Silver"), consumption(0.0), points(0) {}

    // 带参数的构造函数
    Member(int mid, std::string mname, std::string mtype, double mconsumption)
        : id(mid), name(mname), type(mtype), consumption(mconsumption) {
        points = calculatePoints(mconsumption);
    }
    // 积分计算函数：根据消费金额计算积分
    int calculatePoints(double consumptionAmount) {
        // 消费 1 元，获得 10 积分,若消费不足 1 元，按 1 元计算
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
    // 根据鼠标悬停状态改变按钮颜色
    if (isHovered) {
        setfillcolor(LIGHTGRAY);  // 悬停时按钮颜色
        bar(x1, y1, x2, y2);      // 绘制按钮背景
    }
    else {
        setfillcolor(WHITE);      // 默认按钮颜色
        bar(x1, y1, x2, y2);
    }
    setlinecolor(BLACK);          // 按钮边框颜色
    rectangle(x1, y1, x2, y2);    // 绘制按钮边框

    // 设置文字颜色为黄色
    settextcolor(BLACK);
    int textWidth = textwidth(label);
    int textHeight = textheight(label);
    setbkmode(TRANSPARENT);       // 设置文字背景透明
    outtextxy(x1 + (x2 - x1 - textWidth) / 2, y1 + (y2 - y1 - textHeight) / 2, label);  // 绘制按钮文字
}

// 绘制输入框
void drawInputBox(int left, int top, int width, int height, const char* content) {
    setfillstyle(SOLID_FILL, WHITE);
    bar(left, top, left + width, top + height);
    rectangle(left, top, left + width, top + height);
    outtextxy(left + 5, top + 5, content);
}

void updateMemberType(Member& member) {
    if (member.consumption >= 400) {
        member.type = "Gold";  // Change to Gold if consumption > 400
    }
    else {
        member.type = "Silver";  // Otherwise, it stays Silver
    }
}

void addMember() {
    cleardevice();
    putimage(0, 0, &image);
    // 输入框和按钮参数
    int inputBoxWidth = 200;
    int inputBoxHeight = 30;
    int startX = 300;
    int idY = 100, nameY = 200, typeY = 300, consumptionY = 400;
    int buttonWidth = 100, buttonHeight = 50;
    int registerX = startX + (inputBoxWidth - buttonWidth) / 2; // 居中对齐输入框
    int registerY = consumptionY + inputBoxHeight + 50;

    // 输入内容
    std::string idInput = "", nameInput = "", typeInput = "";
    double consumptionInput = 0.0;  // Default consumption amount

    bool idBoxActive = false, nameBoxActive = false, typeBoxActive = false, consumptionBoxActive = false;

    while (true) {
        BeginBatchDraw(); // 开启双缓冲
        cleardevice();
        putimage(0, 0, &image);

        // 绘制标题和静态界面
        settextstyle(50, 0, "华文新魏"); // 设置标题字体和大小
        outtextxy(220, 40, "简单会员管理系统");
        // 恢复默认样式
        settextstyle(20, 0, "Arial");
        outtextxy(startX - 40, idY + 25, "会员ID:");
        outtextxy(startX - 40, nameY + 25, "会员姓名:");
        outtextxy(startX - 40, typeY + 25, "会员类型:");
        outtextxy(startX - 40, consumptionY + 25, "消费金额:");

        // Input boxes
        drawInputBox(startX + 60, idY + 20, inputBoxWidth, inputBoxHeight, idInput.c_str());
        drawInputBox(startX + 60, nameY + 20, inputBoxWidth, inputBoxHeight, nameInput.c_str());
        drawInputBox(startX + 60, typeY + 20, inputBoxWidth, inputBoxHeight, typeInput.c_str());

        // Consumption input box
        char consumptionStr[50];
        sprintf_s(consumptionStr, "%.2f", consumptionInput); // Format consumption input
        drawInputBox(startX + 60, consumptionY + 20, inputBoxWidth, inputBoxHeight, consumptionStr);

        // 按钮状态
        MOUSEMSG msg = GetMouseMsg();
        bool registerHovered = isButtonHovered(registerX, registerY, registerX + buttonWidth, registerY + buttonHeight, msg);
        bool returnHovered = isButtonHovered(650, 500, 750, 550, msg);

        // 绘制按钮，更新颜色
        drawButton(registerX + 10, registerY, registerX + buttonWidth, registerY + buttonHeight, "注册", registerHovered);
        drawButton(650, 500, 750, 550, "返回", returnHovered);

        // 检查鼠标点击事件
        if (msg.uMsg == WM_LBUTTONDOWN) {
            char temp[50] = "";

            // 输入框处理逻辑
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
                        MessageBox(GetHWnd(), "会员类型无效！请输入 Gold 或 Silver。", "错误", MB_OK);
                    }
                }
                else if (msg.y >= consumptionY && msg.y <= consumptionY + inputBoxHeight) {
                    InputBox(temp, 50, "请输入消费金额：");
                    consumptionInput = atof(temp); // Convert input to a floating-point number
                    consumptionBoxActive = true;
                }
            }

            // 注册按钮逻辑
            if (isButtonClicked(registerX, registerY, registerX + buttonWidth, registerY + buttonHeight, msg)) {
                if (idInput.empty() || nameInput.empty() || typeInput.empty()) {
                    MessageBox(GetHWnd(), "所有字段不能为空！", "错误", MB_OK);
                }
                else {
                    // Update the member type based on consumption
                    Member newMember(atoi(idInput.c_str()), nameInput, typeInput, consumptionInput);
                    updateMemberType(newMember);  // Update type based on consumption

                    // MySQL 操作
                    MYSQL* con = mysql_init(nullptr);
                    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8mb4");
                    if (con && mysql_real_connect(con, host, user, pw, database_name, port, nullptr, 0)) {
                        // 检查重复 ID
                        char checkQuery[128];
                        snprintf(checkQuery, sizeof(checkQuery),
                            "SELECT COUNT(*) FROM members WHERE id = '%s'", idInput.c_str());
                        if (mysql_query(con, checkQuery) == 0) {
                            MYSQL_RES* res = mysql_store_result(con);
                            if (res) {
                                MYSQL_ROW row = mysql_fetch_row(res);
                                if (atoi(row[0]) > 0) {
                                    MessageBox(GetHWnd(), "会员 ID 已存在，请重新输入！", "错误", MB_OK);
                                    mysql_free_result(res);
                                    mysql_close(con);
                                    continue;
                                }
                                mysql_free_result(res);
                            }
                        }
                        else {
                            MessageBox(GetHWnd(), mysql_error(con), "数据库查询错误", MB_OK);
                            mysql_close(con);
                            continue;
                        }

                        // 插入会员信息
                        char query[256];
                        snprintf(query, sizeof(query),
                            "INSERT INTO members (id, name, type, consumption) VALUES ('%s', '%s', '%s', %.2f)",
                            idInput.c_str(), nameInput.c_str(), newMember.type.c_str(), newMember.consumption);
                        if (mysql_query(con, query)) {
                            MessageBox(GetHWnd(), mysql_error(con), "数据库插入错误", MB_OK);
                        }
                        else {
                            MessageBox(GetHWnd(), "注册成功！即将返回主页面.....", "成功", MB_OK);
                            mysql_close(con);
                            return;
                        }
                        mysql_close(con);
                    }
                    else {
                        MessageBox(GetHWnd(), "无法连接到数据库！", "错误", MB_OK);
                    }
                }
            }

            // Return 按钮逻辑
            if (isButtonClicked(650, 500, 750, 550, msg)) {
                return;
            }
        }

        EndBatchDraw(); // 更新界面
    }
}


void displayMembers(const std::string& name = "", int currentPage = 1) {
    cleardevice();
    putimage(0, 0, &image);

    const int recordsPerPage = 10;  // 每页显示的记录数
    int totalRecords = 0;           // 总记录数
    int totalPages = 0;             // 总页数

    // 绘制按钮
    drawButton(650, 500, 750, 550, "返回", false);
    drawButton(550, 500, 650, 550, "搜索", false);  // 查找按钮
    drawButton(150, 500, 250, 550, "上一页", false);  // 上一页按钮
    drawButton(300, 500, 400, 550, "下一页", false);  // 下一页按钮

    // 表格参数
    int windowWidth = getwidth();
    int idColumnWidth = 100;   // ID列宽
    int nameColumnWidth = 200; // Name列宽
    int typeColumnWidth = 100; // Type列宽
    int consumptionColumnWidth = 150; // 消费金额列宽
    int pointsColumnWidth = 100; // 积分列宽
    int rowHeight = 30;        // 行高
    int startX = (windowWidth - (idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + pointsColumnWidth)) / 2;
    int startY = 50;           // 表格起始Y坐标
    setlinecolor(BLACK);

    // 添加标题
    settextstyle(45, 0, "华文新魏");
    settextcolor(BLACK);
    outtextxy(260, 0, "会员管理系统");
    settextstyle(20, 0, "Arial");  // 恢复默认样式

    // 绘制标题行
    rectangle(startX, startY + 30, startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + pointsColumnWidth, startY + rowHeight + 30);
    outtextxy(startX + (idColumnWidth - textwidth("ID")) / 2, startY + (rowHeight - textheight("ID")) / 2 + 30, "ID");
    outtextxy(startX + idColumnWidth + (nameColumnWidth - textwidth("姓名")) / 2, startY + (rowHeight - textheight("姓名")) / 2 + 30, "姓名");
    outtextxy(startX + idColumnWidth + nameColumnWidth + (typeColumnWidth - textwidth("类型")) / 2, startY + (rowHeight - textheight("类型")) / 2 + 30, "类型");
    outtextxy(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + (consumptionColumnWidth - textwidth("消费金额")) / 2, startY + (rowHeight - textheight("消费金额")) / 2 + 30, "消费金额");
    outtextxy(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + (pointsColumnWidth - textwidth("积分")) / 2, startY + (rowHeight - textheight("积分")) / 2 + 30, "积分");
    line(startX, startY + rowHeight + 30, startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + pointsColumnWidth, startY + rowHeight + 30);

    // 初始化 MySQL 连接
    MYSQL* con = mysql_init(nullptr);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    if (con == nullptr) {
        MessageBox(GetHWnd(), "MySQL 初始化失败。", "错误", MB_OK);
        return;
    }

    if (mysql_real_connect(con, host, user, pw, database_name, port, nullptr, 0) == nullptr) {
        MessageBox(GetHWnd(), "无法连接到数据库！", "错误", MB_OK);
        mysql_close(con);
        return;
    }
    mysql_set_character_set(con, "utf8mb4");

    // 获取总记录数
    std::string countQuery = "SELECT COUNT(*) FROM members";
    if (!name.empty()) {
        countQuery += " WHERE name = '" + name + "'";
    }
    if (mysql_query(con, countQuery.c_str())) {
        MessageBox(GetHWnd(), mysql_error(con), "获取记录总数失败", MB_OK);
        mysql_close(con);
        return;
    }

    MYSQL_RES* countRes = mysql_store_result(con);
    if (countRes) {
        MYSQL_ROW countRow = mysql_fetch_row(countRes);
        totalRecords = atoi(countRow[0]);
        mysql_free_result(countRes);
    }
    totalPages = (totalRecords + recordsPerPage - 1) / recordsPerPage; // 计算总页数

    // 如果没有记录，提示查无此人
    if (totalRecords == 0) {
        MessageBox(GetHWnd(), "查无此人！", "提示", MB_OK);
        mysql_close(con);
        displayMembers();
        return; // 重新加载当前页面
    }

    // 构造分页查询语句
    int offset = (currentPage - 1) * recordsPerPage;
    std::string query = "SELECT id, name, type, consumption FROM members";
    if (!name.empty()) {
        query += " WHERE name = '" + name + "'";
    }
    query += " ORDER BY id ASC LIMIT " + std::to_string(recordsPerPage) + " OFFSET " + std::to_string(offset);

    if (mysql_query(con, query.c_str())) {
        MessageBox(GetHWnd(), mysql_error(con), "分页查询失败", MB_OK);
        mysql_close(con);
        return;
    }

    MYSQL_RES* res = mysql_store_result(con);
    if (res == nullptr) {
        MessageBox(GetHWnd(), mysql_error(con), "获取查询结果失败", MB_OK);
        mysql_close(con);
        return;
    }

    MYSQL_ROW row;
    int y = startY + rowHeight + 30; // 第一行数据的Y坐标

    // 遍历查询结果，绘制到表格中
    while ((row = mysql_fetch_row(res))) {
        // 更新会员类型
        double consumptionAmount = atof(row[3]);
        std::string memberType = (consumptionAmount >= 400) ? "Gold" : "Silver";

        // 计算积分
        int points = static_cast<int>(std::ceil(consumptionAmount) * 10);

        // 绘制每一行
        rectangle(startX, y, startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + pointsColumnWidth, y + rowHeight);
        line(startX + idColumnWidth, y, startX + idColumnWidth, y + rowHeight);
        line(startX + idColumnWidth + nameColumnWidth, y, startX + idColumnWidth + nameColumnWidth, y + rowHeight);
        line(startX + idColumnWidth + nameColumnWidth + typeColumnWidth, y, startX + idColumnWidth + nameColumnWidth + typeColumnWidth, y + rowHeight);
        line(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth, y, startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth, y + rowHeight);

        // 数据显示
        outtextxy(startX + (idColumnWidth - textwidth(row[0])) / 2, y + (rowHeight - textheight(row[0])) / 2, row[0]);
        outtextxy(startX + idColumnWidth + (nameColumnWidth - textwidth(row[1])) / 2, y + (rowHeight - textheight(row[1])) / 2, row[1]);
        outtextxy(startX + idColumnWidth + nameColumnWidth + (typeColumnWidth - textwidth(memberType.c_str())) / 2, y + (rowHeight - textheight(memberType.c_str())) / 2, memberType.c_str());
        outtextxy(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + (consumptionColumnWidth - textwidth(row[3])) / 2, y + (rowHeight - textheight(row[3])) / 2, row[3]);
        outtextxy(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + (pointsColumnWidth - textwidth(std::to_string(points).c_str())) / 2, y + (rowHeight - textheight(std::to_string(points).c_str())) / 2, std::to_string(points).c_str());

        y += rowHeight; // 移动到下一行
    }

    mysql_free_result(res);
    mysql_close(con);

    // 绘制当前页和总页数信息
    char pageInfo[50];
    sprintf_s(pageInfo, "当前：%d页 / %d页", currentPage, totalPages);
    outtextxy(500, 460, pageInfo);

    // 按钮交互逻辑
    while (true) {
        MOUSEMSG msg = GetMouseMsg();

        // 检查是否点击返回按钮
        if (isButtonClicked(650, 500, 750, 550, msg)) return;

        // 检查是否点击查找按钮
        if (isButtonClicked(550, 500, 650, 550, msg)) {
            char searchName[50];
            InputBox(searchName, 50, "输入要查找的姓名：");
            displayMembers(searchName); // 递归调用过滤显示
            return;
        }

        // 检查是否点击上一页按钮
        if (isButtonClicked(150, 500, 250, 550, msg)) {
            if (currentPage > 1) {
                displayMembers(name, currentPage - 1); // 显示上一页
                return;
            }
        }

        // 检查是否点击下一页按钮
        if (isButtonClicked(300, 500, 400, 550, msg)) {
            if (currentPage < totalPages) {
                displayMembers(name, currentPage + 1); // 显示下一页
                return;
            }
        }
    }
}



void deleteMember() {
    MOUSEMSG msg;

    // 输入要删除的ID
    char idInput[7];
    InputBox(idInput, 7, "请输入要删除的会员ID:");
    int id = atoi(idInput);

    // 检查输入是否为有效的6位数字
    if (!isSixDigitNumber(id)) {
        MessageBox(GetHWnd(), "错误: 会员ID必须是6位数字！", "Error", MB_OK);
        return;
    }

    // 初始化 MySQL 连接
    MYSQL* con = mysql_init(nullptr);
    if (mysql_real_connect(con, host, user, pw, database_name, port, nullptr, 0) == nullptr) {
        MessageBox(GetHWnd(), "数据库连接失败.", "Error", MB_OK);
        mysql_close(con);
        return;
    }

    // 查询会员信息，包括消费金额
    char query[128];
    sprintf_s(query, "SELECT id, name, type, consumption FROM members WHERE id = %d", id);
    if (mysql_query(con, query)) {
        MessageBox(GetHWnd(), "查询失败.", "Error", MB_OK);
        mysql_close(con);
        return;
    }

    MYSQL_RES* res = mysql_store_result(con);
    if (mysql_num_rows(res) == 0) {
        MessageBox(GetHWnd(), "未找到该会员信息.", "Error", MB_OK);
        mysql_free_result(res);
        mysql_close(con);
        return;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    char memberName[50], memberType[20], consumptionAmount[50];
    strcpy_s(memberName, row[1]);
    strcpy_s(memberType, row[2]);
    strcpy_s(consumptionAmount, row[3]); // Retrieve the consumption amount
    mysql_free_result(res);

    // 定义表格和按钮位置
    int tableX = 200, tableY = 150, tableWidth = 400, rowHeight = 50;
    int deleteButtonX1 = 300, deleteButtonY1 = 400, deleteButtonX2 = 500, deleteButtonY2 = 450;
    int returnButtonX1 = 650, returnButtonY1 = 500, returnButtonX2 = 750, returnButtonY2 = 550;

    while (true) {
        // 使用双缓冲绘图防止闪烁
        BeginBatchDraw();
        cleardevice();
        putimage(0, 0, &image);

        // 添加标题
        settextstyle(50, 0, "华文新魏"); // 设置标题字体和大小
        settextcolor(BLACK);          // 设置标题颜色
        outtextxy(260, 60, "会员管理系统"); // 标题内容及位置

        // 绘制表格边框
        setlinecolor(BLACK);
        rectangle(tableX, tableY, tableX + tableWidth, tableY + 4 * rowHeight); // 4 rows now

        // 绘制表格的横线
        line(tableX, tableY + rowHeight, tableX + tableWidth, tableY + rowHeight);
        line(tableX, tableY + 2 * rowHeight, tableX + tableWidth, tableY + 2 * rowHeight);
        line(tableX, tableY + 3 * rowHeight, tableX + tableWidth, tableY + 3 * rowHeight);

        // 绘制表格的竖线
        line(tableX + 100, tableY, tableX + 100, tableY + 4 * rowHeight);

        // 填写表格内容
        settextstyle(20, 0, "Arial");
        outtextxy(tableX + 10, tableY + 10, "会员ID");
        outtextxy(tableX + 110, tableY + 10, idInput);

        outtextxy(tableX + 10, tableY + rowHeight + 10, "会员姓名");
        outtextxy(tableX + 110, tableY + rowHeight + 10, memberName);

        outtextxy(tableX + 10, tableY + 2 * rowHeight + 10, "会员类型");
        outtextxy(tableX + 110, tableY + 2 * rowHeight + 10, memberType);

        outtextxy(tableX + 10, tableY + 3 * rowHeight + 10, "消费金额");
        outtextxy(tableX + 110, tableY + 3 * rowHeight + 10, consumptionAmount);

        // 绘制按钮
        drawButton(deleteButtonX1, deleteButtonY1, deleteButtonX2, deleteButtonY2, "删除", false);
        drawButton(returnButtonX1, returnButtonY1, returnButtonX2, returnButtonY2, "返回", false);

        EndBatchDraw();

        // 处理鼠标事件
        msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN) {
            // 点击“删除”按钮
            if (msg.x > deleteButtonX1 && msg.x < deleteButtonX2 && msg.y > deleteButtonY1 && msg.y < deleteButtonY2) {
                if (MessageBox(GetHWnd(), "确定删除该会员吗？", "提示", MB_OKCANCEL) == IDOK) {
                    char deleteQuery[128];
                    sprintf_s(deleteQuery, "DELETE FROM members WHERE id = %d", id);
                    if (mysql_query(con, deleteQuery)) {
                        MessageBox(GetHWnd(), "删除会员信息失败.", "Error", MB_OK);
                        mysql_close(con);
                        return;
                    }
                    else {
                        MessageBox(GetHWnd(), "会员信息删除成功.", "Success", MB_OK);
                        mysql_close(con);
                        return;
                    }
                }
                else {
                    mysql_close(con);
                    return;
                }
            }
            // 点击“返回”按钮
            else if (msg.x > returnButtonX1 && msg.x < returnButtonX2 && msg.y > returnButtonY1 && msg.y < returnButtonY2) {
                mysql_close(con);
                return;
            }
        }
    }
}


void modifyMember() {
    cleardevice();
    putimage(0, 0, &image);

    // 定义界面布局
    int nameBoxX1 = 300, nameBoxY1 = 200, nameBoxX2 = 200, nameBoxY2 = 30;
    int typeBoxX1 = 300, typeBoxY1 = 300, typeBoxX2 = 200, typeBoxY2 = 30;
    int consumptionBoxX1 = 300, consumptionBoxY1 = 400, consumptionBoxX2 = 200, consumptionBoxY2 = 30;
    int modifyButtonX1 = 300, modifyButtonY1 = 500, modifyButtonX2 = 500, modifyButtonY2 = 550;
    int returnButtonX1 = 650, returnButtonY1 = 500, returnButtonX2 = 750, returnButtonY2 = 550;

    int nameBoxRightX = nameBoxX1 + nameBoxX2;
    int nameBoxBottomY = nameBoxY1 + nameBoxY2; 
    int typeBoxRightX = typeBoxX1 + typeBoxX2; 
    int typeBoxBottomY = typeBoxY1 + typeBoxY2; 
    int consumptionBoxRightX = consumptionBoxX1 + consumptionBoxX2;
    int consumptionBoxBottomY = consumptionBoxY1 + consumptionBoxY2;

    char nameInput[50] = "Enter Name";
    char typeInput[20] = "Enter Type";
    char consumptionInput[20] = "0.00";

    bool refreshRequired = true; // 是否需要刷新界面

    // Initialize MySQL
    MYSQL* con = mysql_init(nullptr);
    if (!mysql_real_connect(con, host, user, pw, database_name, port, nullptr, 0)) {
        MessageBox(GetHWnd(), "无法连接到数据库.", "Error", MB_OK);
        mysql_close(con);
        return;
    }

    // Get the member ID to modify
    char idInput[7];
    InputBox(idInput, 7, "请输入需要修改的会员ID:");
    int id = atoi(idInput);

    // Check if ID is a 6-digit number
    if (!isSixDigitNumber(id)) {
        MessageBox(GetHWnd(), "错误: 会员ID必须是6位数字组成！", "Error", MB_OK);
        mysql_close(con);
        return;
    }

    // Check if the member exists
    char query[128];
    sprintf_s(query, "SELECT name, type, consumption FROM members WHERE id = %d", id);
    if (mysql_query(con, query)) {
        MessageBox(GetHWnd(), "查询失败.", "Error", MB_OK);
        mysql_close(con);
        return;
    }
    MYSQL_RES* res = mysql_store_result(con);
    if (mysql_num_rows(res) == 0) {
        MessageBox(GetHWnd(), "无该ID会员信息.", "Error", MB_OK);
        mysql_free_result(res);
        mysql_close(con);
        return;
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    strcpy_s(nameInput, row[0]);
    strcpy_s(typeInput, row[1]);
    strcpy_s(consumptionInput, row[2]);
    mysql_free_result(res);

    while (true) {
        if (refreshRequired) {
            cleardevice();
            putimage(0, 0, &image);
            outtextxy(nameBoxX1 - 100, nameBoxY1 + 5, "Name:");
            outtextxy(nameBoxX1 - 100, typeBoxY1 + 5, "Type:");
            outtextxy(nameBoxX1 - 100, consumptionBoxY1 + 5, "Consumption:");

            // Add title
            settextstyle(50, 0, "华文新魏");
            settextcolor(BLACK);      
            outtextxy(260, 60, "会员管理系统"); 

            // Restore default style
            settextstyle(20, 0, "Arial");

            // Draw input boxes
            drawInputBox(nameBoxX1, nameBoxY1, nameBoxX2, nameBoxY2, nameInput);
            drawInputBox(typeBoxX1, typeBoxY1, typeBoxX2, typeBoxY2, typeInput);
            drawInputBox(consumptionBoxX1, consumptionBoxY1, consumptionBoxX2, consumptionBoxY2, consumptionInput); // Draw consumption input

            // Draw buttons
            drawButton(modifyButtonX1, modifyButtonY1, modifyButtonX2, modifyButtonY2, "修改", false);
            drawButton(returnButtonX1, returnButtonY1, returnButtonX2, returnButtonY2, "返回", false);

            refreshRequired = false;
        }

        // Handle mouse events
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN) {
            // Click on the name input box
            if (msg.x > nameBoxX1 && msg.x < nameBoxRightX && msg.y > nameBoxY1 && msg.y < nameBoxBottomY) {
                InputBox(nameInput, 50, "输入新的姓名:");
                refreshRequired = true;
            }
            // Click on the type input box
            else if (msg.x > typeBoxX1 && msg.x < typeBoxRightX && msg.y > typeBoxY1 && msg.y < typeBoxBottomY) {
                InputBox(typeInput, 10, "请输入会员类型 (Gold/Silver):");
                refreshRequired = true;
            }
            // Click on the consumption input box
            else if (msg.x > consumptionBoxX1 && msg.x < consumptionBoxRightX && msg.y > consumptionBoxY1 && msg.y < consumptionBoxBottomY) {
                InputBox(consumptionInput, 20, "请输入新的消费金额:");
                refreshRequired = true;
            }
            // Click on the modify button
            else if (msg.x > modifyButtonX1 && msg.x < modifyButtonX2 && msg.y > modifyButtonY1 && msg.y < modifyButtonY2) {
                // Validate the type input
                if (strcmp(typeInput, "Gold") != 0 && strcmp(typeInput, "Silver") != 0) {
                    MessageBox(GetHWnd(), "错误: 会员类型必须是 'Gold' 或者 'Silver'.", "Error", MB_OK);
                }
                else {
                    // Validate the consumption value
                    double consumptionValue = atof(consumptionInput);
                    if (consumptionValue < 0) {
                        MessageBox(GetHWnd(), "消费金额必须为正数.", "Error", MB_OK);
                    }
                    else {
                        if (MessageBox(GetHWnd(), "确定修改吗？", "提示", MB_OKCANCEL) == IDOK) {
                            // Update the database
                            char updateQuery[256];
                            sprintf_s(updateQuery, "UPDATE members SET name = '%s', type = '%s', consumption = '%f' WHERE id = %d",
                                nameInput, typeInput, consumptionValue, id);
                            if (mysql_query(con, updateQuery)) {
                                MessageBox(GetHWnd(), "会员信息更新失败.", "Error", MB_OK);
                                break;
                            }
                            else {
                                // Update the member type based on consumption
                                Member updatedMember;
                                updatedMember.name = nameInput;
                                updatedMember.type = typeInput;
                                updatedMember.consumption = consumptionValue;

                                updateMemberType(updatedMember);

                                // Update the member type in the database
                                char finalUpdateQuery[256];
                                sprintf_s(finalUpdateQuery, "UPDATE members SET type = '%s' WHERE id = %d", updatedMember.type.c_str(), id);
                                if (mysql_query(con, finalUpdateQuery)) {
                                    std::cerr << "MySQL query error: " << mysql_error(con) << std::endl;
                                    MessageBox(GetHWnd(), "更新会员类型失败.", "Error", MB_OK);
                                }
                                else {
                                    MessageBox(GetHWnd(), "会员信息更新成功.", "Success", MB_OK);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            // 点击返回按钮
            else if (msg.x > returnButtonX1 && msg.x < returnButtonX2 && msg.y > returnButtonY1 && msg.y < returnButtonY2) {
                mysql_close(con);
                return;
            }
        }
    }
}

void countMembersByType() {
    // 初始化参数
    int dropdownX1 = 300, dropdownY1 = 100, dropdownX2 = 500, dropdownY2 = 130;
    int dropdownOptionHeight = 30;
    std::string selectedType = "选择会员类型";
    bool dropdownOpen = false;
    int page = 1, rowsPerPage = 6;
    int totalPages = 1;
    int totalMembers = 0;

    bool refreshRequired = true; // 用于控制是否刷新显示

    auto drawDropdown = [&]() {
        setfillcolor(LIGHTGRAY);
        bar(dropdownX1, dropdownY1, dropdownX2, dropdownY2);
        setlinecolor(BLACK);
        rectangle(dropdownX1, dropdownY1, dropdownX2, dropdownY2);
        outtextxy(dropdownX1 + 10, dropdownY1 + (dropdownY2 - dropdownY1 - textheight(selectedType.c_str())) / 2, selectedType.c_str());

        if (dropdownOpen) {
            setfillcolor(WHITE);
            bar(dropdownX1, dropdownY2, dropdownX2, dropdownY2 + 2 * dropdownOptionHeight);
            rectangle(dropdownX1, dropdownY2, dropdownX2, dropdownY2 + 2 * dropdownOptionHeight);
            outtextxy(dropdownX1 + 10, dropdownY2 + (dropdownOptionHeight - textheight("Gold")) / 2, "Gold");
            outtextxy(dropdownX1 + 10, dropdownY2 + dropdownOptionHeight + (dropdownOptionHeight - textheight("Silver")) / 2, "Silver");
        }
    };

    auto drawPaginationInfo = [&]() {
        char pageInfo[50];
        sprintf_s(pageInfo, "当前：%d页 / %d页", page, totalPages);
        int pageX = getwidth() - textwidth(pageInfo) - 20;
        int pageY = getheight() - textheight(pageInfo) - 20;
        settextcolor(WHITE);
        outtextxy(pageX, pageY, pageInfo);
    };

    auto drawTable = [&](MYSQL_RES* res) {
        int windowWidth = getwidth();
        int idColumnWidth = 100;
        int nameColumnWidth = 200;
        int rowHeight = 30;

        int startX = (windowWidth - (idColumnWidth + nameColumnWidth)) / 2;
        int startY = 200;
        // 添加标题
        settextstyle(50, 0, "华文新魏"); // 设置标题字体和大小
        settextcolor(BLACK);          // 设置标题颜色
        outtextxy(260, 30, "会员管理系统"); // 标题内容及位置
        // 恢复默认样式
        settextstyle(20, 0, "Arial");
        // 绘制标题行
        setlinecolor(BLACK);
        rectangle(startX, startY - 20, startX + idColumnWidth + nameColumnWidth, startY + rowHeight - 20);
        outtextxy(startX + (idColumnWidth - textwidth("会员ID")) / 2, startY + (rowHeight - textheight("会员ID")) / 2 - 20, "会员ID");
        outtextxy(startX + idColumnWidth + (nameColumnWidth - textwidth("会员姓名")) / 2, startY + (rowHeight - textheight("会员姓名")) / 2 - 20, "会员姓名");
        line(startX, startY + rowHeight - 20, startX + idColumnWidth + nameColumnWidth, startY + rowHeight - 20);

        // 绘制数据行
        MYSQL_ROW row;
        int y = startY + rowHeight - 20;
        while ((row = mysql_fetch_row(res))) {
            rectangle(startX, y, startX + idColumnWidth + nameColumnWidth, y + rowHeight);
            line(startX + idColumnWidth, y, startX + idColumnWidth, y + rowHeight);

            outtextxy(startX + (idColumnWidth - textwidth(row[0])) / 2, y + (rowHeight - textheight(row[0])) / 2, row[0]);
            outtextxy(startX + idColumnWidth + (nameColumnWidth - textwidth(row[1])) / 2, y + (rowHeight - textheight(row[1])) / 2, row[1]);

            y += rowHeight;
        }

        // 绘制统计信息
        rectangle(startX, y, startX + idColumnWidth + nameColumnWidth, y + rowHeight);
        char totalDisplay[50];
        sprintf_s(totalDisplay, "总共有 %s 会员: %d人", selectedType.c_str(), totalMembers);
        outtextxy(startX + (idColumnWidth + nameColumnWidth - textwidth(totalDisplay)) / 2, y + (rowHeight - textheight(totalDisplay)) / 2, totalDisplay);
    };

    while (true) {
        // 检测鼠标点击
        MOUSEMSG msg = GetMouseMsg();

        if (isButtonClicked(650, 500, 750, 550, msg)) return;

        if (msg.uMsg == WM_LBUTTONDOWN) {
            if (msg.x > dropdownX1 && msg.x < dropdownX2 && msg.y > dropdownY1 && msg.y < dropdownY2) {
                dropdownOpen = !dropdownOpen;
                refreshRequired = true;
            }
            else if (dropdownOpen) {
                if (msg.x > dropdownX1 && msg.x < dropdownX2 && msg.y > dropdownY2 && msg.y < dropdownY2 + dropdownOptionHeight) {
                    selectedType = "Gold";
                    dropdownOpen = false;
                    page = 1;
                    refreshRequired = true;
                }
                else if (msg.x > dropdownX1 && msg.x < dropdownX2 && msg.y > dropdownY2 + dropdownOptionHeight && msg.y < dropdownY2 + 2 * dropdownOptionHeight) {
                    selectedType = "Silver";
                    dropdownOpen = false;
                    page = 1;
                    refreshRequired = true;
                }
            }
        }

        if (refreshRequired) {
            MYSQL* con = mysql_init(nullptr);
            if (mysql_real_connect(con, host, user, pw, database_name, port, nullptr, 0)) {
                char countQuery[128];
                sprintf_s(countQuery, "SELECT COUNT(*) FROM members WHERE type = '%s'", selectedType.c_str());
                mysql_query(con, countQuery);
                MYSQL_RES* countRes = mysql_store_result(con);
                MYSQL_ROW countRow = mysql_fetch_row(countRes);
                totalMembers = atoi(countRow[0]);
                mysql_free_result(countRes);

                totalPages = (totalMembers + rowsPerPage - 1) / rowsPerPage;

                int offset = (page - 1) * rowsPerPage;
                char query[256];
                sprintf_s(query, "SELECT id, name FROM members WHERE type = '%s' LIMIT %d OFFSET %d", selectedType.c_str(), rowsPerPage, offset);
                mysql_query(con, query);
                MYSQL_RES* res = mysql_store_result(con);

                cleardevice();
                putimage(0, 0, &image);
                drawDropdown();
                drawButton(650, 500, 750, 550, "返回", false);
                if (page > 1) drawButton(150, 500, 250, 550, "上一页", false);
                if (page < totalPages) drawButton(300, 500, 400, 550, "下一页", false);
                drawPaginationInfo();
                drawTable(res);

                mysql_free_result(res);
                mysql_close(con);
            }

            refreshRequired = false;
        }

        if (isButtonClicked(150, 500, 250, 550, msg) && page > 1) {
            page--;
            refreshRequired = true;
        }
        else if (isButtonClicked(300, 500, 400, 550, msg) && page < totalPages) {
            page++;
            refreshRequired = true;
        }
    }
}

void drawTitle(const char* title) {
    // 绘制居中标题
    int windowWidth = getwidth(); // 获取窗口宽度
    int titleWidth = textwidth(title);
    int titleHeight = textheight(title);

    settextcolor(BLACK);
    setbkmode(TRANSPARENT);  // 设置背景透明
    settextstyle(50, 0, "华文新魏"); // 设置标题字体和大小
    outtextxy((windowWidth - titleWidth) / 2 - 100, 20, title); // 居中显示标题
    settextstyle(20, 0, "Arial");
}

void showInstructionsAndExitIfNotConfirmed() {
    // 使用 MessageBox 显示使用说明
    int msgResult = MessageBox(GetHWnd(), "欢迎使用会员管理系统！\n\n"
        "1. 添加会员：用于添加新会员。\n"
        "2. 查看会员：查看当前系统中的所有会员。\n"
        "3. 删除会员：删除指定的会员。\n"
        "4. 修改会员：修改会员的基本信息。\n"
        "5. 按类别统计会员：按会员类型查看统计信息。\n\n"
        "点击确定继续使用，点击取消退出系统。",
        "系统使用说明", MB_OKCANCEL | MB_ICONINFORMATION);

    // 判断用户的选择
    if (msgResult == IDCANCEL) {
        // 如果用户点击取消，则退出程序
        exit(0);
    }
}

void showMenu() {
    while (true) {
        // 使用双缓冲，避免闪烁
        BeginBatchDraw();

        cleardevice();
        putimage(0, 0, &image);
        // 绘制标题
        drawTitle("简单会员管理系统");

        MOUSEMSG msg = GetMouseMsg();

        // 判断按钮是否被悬停
        bool addHovered = isButtonHovered(300, 100, 500, 150, msg);
        bool displayHovered = isButtonHovered(300, 160, 500, 210, msg);
        bool deleteHovered = isButtonHovered(300, 220, 500, 270, msg);
        bool modifyHovered = isButtonHovered(300, 280, 500, 330, msg);
        bool countHovered = isButtonHovered(300, 340, 500, 390, msg);
        bool exitHovered = isButtonHovered(300, 400, 500, 450, msg);

        // 绘制按钮，并根据悬停状态改变颜色
        drawButton(300, 100, 500, 150, "添加会员信息", addHovered);
        drawButton(300, 160, 500, 210, "查询所有会员", displayHovered);
        drawButton(300, 220, 500, 270, "删除会员信息", deleteHovered);
        drawButton(300, 280, 500, 330, "修改会员信息", modifyHovered);
        drawButton(300, 340, 500, 390, "会员类型统计", countHovered);
        drawButton(300, 400, 500, 450, "退出应用程序", exitHovered);

        EndBatchDraw();  // 双缓冲结束，更新画面

        // 处理按钮点击事件
        if (isButtonClicked(300, 100, 500, 150, msg)) addMember();
        else if (isButtonClicked(300, 160, 500, 210, msg)) displayMembers();
        else if (isButtonClicked(300, 220, 500, 270, msg)) deleteMember();
        else if (isButtonClicked(300, 280, 500, 330, msg)) modifyMember();
        else if (isButtonClicked(300, 340, 500, 390, msg)) countMembersByType();
        else if (isButtonClicked(300, 400, 500, 450, msg)) {
            if (MessageBox(GetHWnd(), "确定退出本系统吗？", "提示", MB_OKCANCEL) == IDOK) {
                // 如果用户点击"OK"，则退出循环或执行退出操作
                break;
            }
            else {
                continue;
            }
        }
    }
}

int main() {
    // 窗口
    HWND hwndConsole = GetConsoleWindow();
    ShowWindow(hwndConsole, SW_HIDE);
    initgraph(800, 600);
    HWND hwnd = GetHWnd();
    // 使用说明
    showInstructionsAndExitIfNotConfirmed();
    SetWindowText(hwnd, "简单会员管理系统V1.0");
    // 窗口图标背景
    HICON hIcon = (HICON)LoadImage(NULL, APP_ICON, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    loadimage(&image, "./image.png", 800, 600);
    // 菜单
    showMenu();
    closegraph();
    DestroyIcon(hIcon);
    return 0;
}