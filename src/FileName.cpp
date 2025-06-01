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

    // Ĭ�Ϲ��캯��
    Member() : id(0), name(""), type("Silver"), consumption(0.0), points(0) {}

    // �������Ĺ��캯��
    Member(int mid, std::string mname, std::string mtype, double mconsumption)
        : id(mid), name(mname), type(mtype), consumption(mconsumption) {
        points = calculatePoints(mconsumption);
    }
    // ���ּ��㺯�����������ѽ��������
    int calculatePoints(double consumptionAmount) {
        // ���� 1 Ԫ����� 10 ����,�����Ѳ��� 1 Ԫ���� 1 Ԫ����
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
    // ���������ͣ״̬�ı䰴ť��ɫ
    if (isHovered) {
        setfillcolor(LIGHTGRAY);  // ��ͣʱ��ť��ɫ
        bar(x1, y1, x2, y2);      // ���ư�ť����
    }
    else {
        setfillcolor(WHITE);      // Ĭ�ϰ�ť��ɫ
        bar(x1, y1, x2, y2);
    }
    setlinecolor(BLACK);          // ��ť�߿���ɫ
    rectangle(x1, y1, x2, y2);    // ���ư�ť�߿�

    // ����������ɫΪ��ɫ
    settextcolor(BLACK);
    int textWidth = textwidth(label);
    int textHeight = textheight(label);
    setbkmode(TRANSPARENT);       // �������ֱ���͸��
    outtextxy(x1 + (x2 - x1 - textWidth) / 2, y1 + (y2 - y1 - textHeight) / 2, label);  // ���ư�ť����
}

// ���������
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
    // �����Ͱ�ť����
    int inputBoxWidth = 200;
    int inputBoxHeight = 30;
    int startX = 300;
    int idY = 100, nameY = 200, typeY = 300, consumptionY = 400;
    int buttonWidth = 100, buttonHeight = 50;
    int registerX = startX + (inputBoxWidth - buttonWidth) / 2; // ���ж��������
    int registerY = consumptionY + inputBoxHeight + 50;

    // ��������
    std::string idInput = "", nameInput = "", typeInput = "";
    double consumptionInput = 0.0;  // Default consumption amount

    bool idBoxActive = false, nameBoxActive = false, typeBoxActive = false, consumptionBoxActive = false;

    while (true) {
        BeginBatchDraw(); // ����˫����
        cleardevice();
        putimage(0, 0, &image);

        // ���Ʊ���;�̬����
        settextstyle(50, 0, "������κ"); // ���ñ�������ʹ�С
        outtextxy(220, 40, "�򵥻�Ա����ϵͳ");
        // �ָ�Ĭ����ʽ
        settextstyle(20, 0, "Arial");
        outtextxy(startX - 40, idY + 25, "��ԱID:");
        outtextxy(startX - 40, nameY + 25, "��Ա����:");
        outtextxy(startX - 40, typeY + 25, "��Ա����:");
        outtextxy(startX - 40, consumptionY + 25, "���ѽ��:");

        // Input boxes
        drawInputBox(startX + 60, idY + 20, inputBoxWidth, inputBoxHeight, idInput.c_str());
        drawInputBox(startX + 60, nameY + 20, inputBoxWidth, inputBoxHeight, nameInput.c_str());
        drawInputBox(startX + 60, typeY + 20, inputBoxWidth, inputBoxHeight, typeInput.c_str());

        // Consumption input box
        char consumptionStr[50];
        sprintf_s(consumptionStr, "%.2f", consumptionInput); // Format consumption input
        drawInputBox(startX + 60, consumptionY + 20, inputBoxWidth, inputBoxHeight, consumptionStr);

        // ��ť״̬
        MOUSEMSG msg = GetMouseMsg();
        bool registerHovered = isButtonHovered(registerX, registerY, registerX + buttonWidth, registerY + buttonHeight, msg);
        bool returnHovered = isButtonHovered(650, 500, 750, 550, msg);

        // ���ư�ť��������ɫ
        drawButton(registerX + 10, registerY, registerX + buttonWidth, registerY + buttonHeight, "ע��", registerHovered);
        drawButton(650, 500, 750, 550, "����", returnHovered);

        // ���������¼�
        if (msg.uMsg == WM_LBUTTONDOWN) {
            char temp[50] = "";

            // ��������߼�
            if (msg.x >= startX && msg.x <= startX + inputBoxWidth) {
                if (msg.y >= idY && msg.y <= idY + inputBoxHeight) {
                    InputBox(temp, 7, "�������Ա ID��6 λ���֣���");
                    if (strlen(temp) > 0 && isSixDigitNumber(atoi(temp))) {
                        idInput = temp;
                        idBoxActive = true;
                    }
                    else {
                        MessageBox(GetHWnd(), "��Ա ID ������ 6 λ���֣�", "����", MB_OK);
                    }
                }
                else if (msg.y >= nameY && msg.y <= nameY + inputBoxHeight) {
                    InputBox(temp, 50, "�������Ա������");
                    if (strlen(temp) > 0) {
                        nameInput = temp;
                        nameBoxActive = true;
                    }
                    else {
                        MessageBox(GetHWnd(), "��������Ϊ�գ�", "����", MB_OK);
                    }
                }
                else if (msg.y >= typeY && msg.y <= typeY + inputBoxHeight) {
                    InputBox(temp, 10, "�������Ա���ͣ�Gold/Silver����");
                    if (std::string(temp) == "Gold" || std::string(temp) == "Silver") {
                        typeInput = temp;
                        typeBoxActive = true;
                    }
                    else {
                        MessageBox(GetHWnd(), "��Ա������Ч�������� Gold �� Silver��", "����", MB_OK);
                    }
                }
                else if (msg.y >= consumptionY && msg.y <= consumptionY + inputBoxHeight) {
                    InputBox(temp, 50, "���������ѽ�");
                    consumptionInput = atof(temp); // Convert input to a floating-point number
                    consumptionBoxActive = true;
                }
            }

            // ע�ᰴť�߼�
            if (isButtonClicked(registerX, registerY, registerX + buttonWidth, registerY + buttonHeight, msg)) {
                if (idInput.empty() || nameInput.empty() || typeInput.empty()) {
                    MessageBox(GetHWnd(), "�����ֶβ���Ϊ�գ�", "����", MB_OK);
                }
                else {
                    // Update the member type based on consumption
                    Member newMember(atoi(idInput.c_str()), nameInput, typeInput, consumptionInput);
                    updateMemberType(newMember);  // Update type based on consumption

                    // MySQL ����
                    MYSQL* con = mysql_init(nullptr);
                    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8mb4");
                    if (con && mysql_real_connect(con, host, user, pw, database_name, port, nullptr, 0)) {
                        // ����ظ� ID
                        char checkQuery[128];
                        snprintf(checkQuery, sizeof(checkQuery),
                            "SELECT COUNT(*) FROM members WHERE id = '%s'", idInput.c_str());
                        if (mysql_query(con, checkQuery) == 0) {
                            MYSQL_RES* res = mysql_store_result(con);
                            if (res) {
                                MYSQL_ROW row = mysql_fetch_row(res);
                                if (atoi(row[0]) > 0) {
                                    MessageBox(GetHWnd(), "��Ա ID �Ѵ��ڣ����������룡", "����", MB_OK);
                                    mysql_free_result(res);
                                    mysql_close(con);
                                    continue;
                                }
                                mysql_free_result(res);
                            }
                        }
                        else {
                            MessageBox(GetHWnd(), mysql_error(con), "���ݿ��ѯ����", MB_OK);
                            mysql_close(con);
                            continue;
                        }

                        // �����Ա��Ϣ
                        char query[256];
                        snprintf(query, sizeof(query),
                            "INSERT INTO members (id, name, type, consumption) VALUES ('%s', '%s', '%s', %.2f)",
                            idInput.c_str(), nameInput.c_str(), newMember.type.c_str(), newMember.consumption);
                        if (mysql_query(con, query)) {
                            MessageBox(GetHWnd(), mysql_error(con), "���ݿ�������", MB_OK);
                        }
                        else {
                            MessageBox(GetHWnd(), "ע��ɹ�������������ҳ��.....", "�ɹ�", MB_OK);
                            mysql_close(con);
                            return;
                        }
                        mysql_close(con);
                    }
                    else {
                        MessageBox(GetHWnd(), "�޷����ӵ����ݿ⣡", "����", MB_OK);
                    }
                }
            }

            // Return ��ť�߼�
            if (isButtonClicked(650, 500, 750, 550, msg)) {
                return;
            }
        }

        EndBatchDraw(); // ���½���
    }
}


void displayMembers(const std::string& name = "", int currentPage = 1) {
    cleardevice();
    putimage(0, 0, &image);

    const int recordsPerPage = 10;  // ÿҳ��ʾ�ļ�¼��
    int totalRecords = 0;           // �ܼ�¼��
    int totalPages = 0;             // ��ҳ��

    // ���ư�ť
    drawButton(650, 500, 750, 550, "����", false);
    drawButton(550, 500, 650, 550, "����", false);  // ���Ұ�ť
    drawButton(150, 500, 250, 550, "��һҳ", false);  // ��һҳ��ť
    drawButton(300, 500, 400, 550, "��һҳ", false);  // ��һҳ��ť

    // ������
    int windowWidth = getwidth();
    int idColumnWidth = 100;   // ID�п�
    int nameColumnWidth = 200; // Name�п�
    int typeColumnWidth = 100; // Type�п�
    int consumptionColumnWidth = 150; // ���ѽ���п�
    int pointsColumnWidth = 100; // �����п�
    int rowHeight = 30;        // �и�
    int startX = (windowWidth - (idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + pointsColumnWidth)) / 2;
    int startY = 50;           // �����ʼY����
    setlinecolor(BLACK);

    // ��ӱ���
    settextstyle(45, 0, "������κ");
    settextcolor(BLACK);
    outtextxy(260, 0, "��Ա����ϵͳ");
    settextstyle(20, 0, "Arial");  // �ָ�Ĭ����ʽ

    // ���Ʊ�����
    rectangle(startX, startY + 30, startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + pointsColumnWidth, startY + rowHeight + 30);
    outtextxy(startX + (idColumnWidth - textwidth("ID")) / 2, startY + (rowHeight - textheight("ID")) / 2 + 30, "ID");
    outtextxy(startX + idColumnWidth + (nameColumnWidth - textwidth("����")) / 2, startY + (rowHeight - textheight("����")) / 2 + 30, "����");
    outtextxy(startX + idColumnWidth + nameColumnWidth + (typeColumnWidth - textwidth("����")) / 2, startY + (rowHeight - textheight("����")) / 2 + 30, "����");
    outtextxy(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + (consumptionColumnWidth - textwidth("���ѽ��")) / 2, startY + (rowHeight - textheight("���ѽ��")) / 2 + 30, "���ѽ��");
    outtextxy(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + (pointsColumnWidth - textwidth("����")) / 2, startY + (rowHeight - textheight("����")) / 2 + 30, "����");
    line(startX, startY + rowHeight + 30, startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + pointsColumnWidth, startY + rowHeight + 30);

    // ��ʼ�� MySQL ����
    MYSQL* con = mysql_init(nullptr);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    if (con == nullptr) {
        MessageBox(GetHWnd(), "MySQL ��ʼ��ʧ�ܡ�", "����", MB_OK);
        return;
    }

    if (mysql_real_connect(con, host, user, pw, database_name, port, nullptr, 0) == nullptr) {
        MessageBox(GetHWnd(), "�޷����ӵ����ݿ⣡", "����", MB_OK);
        mysql_close(con);
        return;
    }
    mysql_set_character_set(con, "utf8mb4");

    // ��ȡ�ܼ�¼��
    std::string countQuery = "SELECT COUNT(*) FROM members";
    if (!name.empty()) {
        countQuery += " WHERE name = '" + name + "'";
    }
    if (mysql_query(con, countQuery.c_str())) {
        MessageBox(GetHWnd(), mysql_error(con), "��ȡ��¼����ʧ��", MB_OK);
        mysql_close(con);
        return;
    }

    MYSQL_RES* countRes = mysql_store_result(con);
    if (countRes) {
        MYSQL_ROW countRow = mysql_fetch_row(countRes);
        totalRecords = atoi(countRow[0]);
        mysql_free_result(countRes);
    }
    totalPages = (totalRecords + recordsPerPage - 1) / recordsPerPage; // ������ҳ��

    // ���û�м�¼����ʾ���޴���
    if (totalRecords == 0) {
        MessageBox(GetHWnd(), "���޴��ˣ�", "��ʾ", MB_OK);
        mysql_close(con);
        displayMembers();
        return; // ���¼��ص�ǰҳ��
    }

    // �����ҳ��ѯ���
    int offset = (currentPage - 1) * recordsPerPage;
    std::string query = "SELECT id, name, type, consumption FROM members";
    if (!name.empty()) {
        query += " WHERE name = '" + name + "'";
    }
    query += " ORDER BY id ASC LIMIT " + std::to_string(recordsPerPage) + " OFFSET " + std::to_string(offset);

    if (mysql_query(con, query.c_str())) {
        MessageBox(GetHWnd(), mysql_error(con), "��ҳ��ѯʧ��", MB_OK);
        mysql_close(con);
        return;
    }

    MYSQL_RES* res = mysql_store_result(con);
    if (res == nullptr) {
        MessageBox(GetHWnd(), mysql_error(con), "��ȡ��ѯ���ʧ��", MB_OK);
        mysql_close(con);
        return;
    }

    MYSQL_ROW row;
    int y = startY + rowHeight + 30; // ��һ�����ݵ�Y����

    // ������ѯ��������Ƶ������
    while ((row = mysql_fetch_row(res))) {
        // ���»�Ա����
        double consumptionAmount = atof(row[3]);
        std::string memberType = (consumptionAmount >= 400) ? "Gold" : "Silver";

        // �������
        int points = static_cast<int>(std::ceil(consumptionAmount) * 10);

        // ����ÿһ��
        rectangle(startX, y, startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + pointsColumnWidth, y + rowHeight);
        line(startX + idColumnWidth, y, startX + idColumnWidth, y + rowHeight);
        line(startX + idColumnWidth + nameColumnWidth, y, startX + idColumnWidth + nameColumnWidth, y + rowHeight);
        line(startX + idColumnWidth + nameColumnWidth + typeColumnWidth, y, startX + idColumnWidth + nameColumnWidth + typeColumnWidth, y + rowHeight);
        line(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth, y, startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth, y + rowHeight);

        // ������ʾ
        outtextxy(startX + (idColumnWidth - textwidth(row[0])) / 2, y + (rowHeight - textheight(row[0])) / 2, row[0]);
        outtextxy(startX + idColumnWidth + (nameColumnWidth - textwidth(row[1])) / 2, y + (rowHeight - textheight(row[1])) / 2, row[1]);
        outtextxy(startX + idColumnWidth + nameColumnWidth + (typeColumnWidth - textwidth(memberType.c_str())) / 2, y + (rowHeight - textheight(memberType.c_str())) / 2, memberType.c_str());
        outtextxy(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + (consumptionColumnWidth - textwidth(row[3])) / 2, y + (rowHeight - textheight(row[3])) / 2, row[3]);
        outtextxy(startX + idColumnWidth + nameColumnWidth + typeColumnWidth + consumptionColumnWidth + (pointsColumnWidth - textwidth(std::to_string(points).c_str())) / 2, y + (rowHeight - textheight(std::to_string(points).c_str())) / 2, std::to_string(points).c_str());

        y += rowHeight; // �ƶ�����һ��
    }

    mysql_free_result(res);
    mysql_close(con);

    // ���Ƶ�ǰҳ����ҳ����Ϣ
    char pageInfo[50];
    sprintf_s(pageInfo, "��ǰ��%dҳ / %dҳ", currentPage, totalPages);
    outtextxy(500, 460, pageInfo);

    // ��ť�����߼�
    while (true) {
        MOUSEMSG msg = GetMouseMsg();

        // ����Ƿ������ذ�ť
        if (isButtonClicked(650, 500, 750, 550, msg)) return;

        // ����Ƿ������Ұ�ť
        if (isButtonClicked(550, 500, 650, 550, msg)) {
            char searchName[50];
            InputBox(searchName, 50, "����Ҫ���ҵ�������");
            displayMembers(searchName); // �ݹ���ù�����ʾ
            return;
        }

        // ����Ƿ�����һҳ��ť
        if (isButtonClicked(150, 500, 250, 550, msg)) {
            if (currentPage > 1) {
                displayMembers(name, currentPage - 1); // ��ʾ��һҳ
                return;
            }
        }

        // ����Ƿ�����һҳ��ť
        if (isButtonClicked(300, 500, 400, 550, msg)) {
            if (currentPage < totalPages) {
                displayMembers(name, currentPage + 1); // ��ʾ��һҳ
                return;
            }
        }
    }
}



void deleteMember() {
    MOUSEMSG msg;

    // ����Ҫɾ����ID
    char idInput[7];
    InputBox(idInput, 7, "������Ҫɾ���Ļ�ԱID:");
    int id = atoi(idInput);

    // ��������Ƿ�Ϊ��Ч��6λ����
    if (!isSixDigitNumber(id)) {
        MessageBox(GetHWnd(), "����: ��ԱID������6λ���֣�", "Error", MB_OK);
        return;
    }

    // ��ʼ�� MySQL ����
    MYSQL* con = mysql_init(nullptr);
    if (mysql_real_connect(con, host, user, pw, database_name, port, nullptr, 0) == nullptr) {
        MessageBox(GetHWnd(), "���ݿ�����ʧ��.", "Error", MB_OK);
        mysql_close(con);
        return;
    }

    // ��ѯ��Ա��Ϣ���������ѽ��
    char query[128];
    sprintf_s(query, "SELECT id, name, type, consumption FROM members WHERE id = %d", id);
    if (mysql_query(con, query)) {
        MessageBox(GetHWnd(), "��ѯʧ��.", "Error", MB_OK);
        mysql_close(con);
        return;
    }

    MYSQL_RES* res = mysql_store_result(con);
    if (mysql_num_rows(res) == 0) {
        MessageBox(GetHWnd(), "δ�ҵ��û�Ա��Ϣ.", "Error", MB_OK);
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

    // ������Ͱ�ťλ��
    int tableX = 200, tableY = 150, tableWidth = 400, rowHeight = 50;
    int deleteButtonX1 = 300, deleteButtonY1 = 400, deleteButtonX2 = 500, deleteButtonY2 = 450;
    int returnButtonX1 = 650, returnButtonY1 = 500, returnButtonX2 = 750, returnButtonY2 = 550;

    while (true) {
        // ʹ��˫�����ͼ��ֹ��˸
        BeginBatchDraw();
        cleardevice();
        putimage(0, 0, &image);

        // ��ӱ���
        settextstyle(50, 0, "������κ"); // ���ñ�������ʹ�С
        settextcolor(BLACK);          // ���ñ�����ɫ
        outtextxy(260, 60, "��Ա����ϵͳ"); // �������ݼ�λ��

        // ���Ʊ��߿�
        setlinecolor(BLACK);
        rectangle(tableX, tableY, tableX + tableWidth, tableY + 4 * rowHeight); // 4 rows now

        // ���Ʊ��ĺ���
        line(tableX, tableY + rowHeight, tableX + tableWidth, tableY + rowHeight);
        line(tableX, tableY + 2 * rowHeight, tableX + tableWidth, tableY + 2 * rowHeight);
        line(tableX, tableY + 3 * rowHeight, tableX + tableWidth, tableY + 3 * rowHeight);

        // ���Ʊ�������
        line(tableX + 100, tableY, tableX + 100, tableY + 4 * rowHeight);

        // ��д�������
        settextstyle(20, 0, "Arial");
        outtextxy(tableX + 10, tableY + 10, "��ԱID");
        outtextxy(tableX + 110, tableY + 10, idInput);

        outtextxy(tableX + 10, tableY + rowHeight + 10, "��Ա����");
        outtextxy(tableX + 110, tableY + rowHeight + 10, memberName);

        outtextxy(tableX + 10, tableY + 2 * rowHeight + 10, "��Ա����");
        outtextxy(tableX + 110, tableY + 2 * rowHeight + 10, memberType);

        outtextxy(tableX + 10, tableY + 3 * rowHeight + 10, "���ѽ��");
        outtextxy(tableX + 110, tableY + 3 * rowHeight + 10, consumptionAmount);

        // ���ư�ť
        drawButton(deleteButtonX1, deleteButtonY1, deleteButtonX2, deleteButtonY2, "ɾ��", false);
        drawButton(returnButtonX1, returnButtonY1, returnButtonX2, returnButtonY2, "����", false);

        EndBatchDraw();

        // ��������¼�
        msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN) {
            // �����ɾ������ť
            if (msg.x > deleteButtonX1 && msg.x < deleteButtonX2 && msg.y > deleteButtonY1 && msg.y < deleteButtonY2) {
                if (MessageBox(GetHWnd(), "ȷ��ɾ���û�Ա��", "��ʾ", MB_OKCANCEL) == IDOK) {
                    char deleteQuery[128];
                    sprintf_s(deleteQuery, "DELETE FROM members WHERE id = %d", id);
                    if (mysql_query(con, deleteQuery)) {
                        MessageBox(GetHWnd(), "ɾ����Ա��Ϣʧ��.", "Error", MB_OK);
                        mysql_close(con);
                        return;
                    }
                    else {
                        MessageBox(GetHWnd(), "��Ա��Ϣɾ���ɹ�.", "Success", MB_OK);
                        mysql_close(con);
                        return;
                    }
                }
                else {
                    mysql_close(con);
                    return;
                }
            }
            // ��������ء���ť
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

    // ������沼��
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

    bool refreshRequired = true; // �Ƿ���Ҫˢ�½���

    // Initialize MySQL
    MYSQL* con = mysql_init(nullptr);
    if (!mysql_real_connect(con, host, user, pw, database_name, port, nullptr, 0)) {
        MessageBox(GetHWnd(), "�޷����ӵ����ݿ�.", "Error", MB_OK);
        mysql_close(con);
        return;
    }

    // Get the member ID to modify
    char idInput[7];
    InputBox(idInput, 7, "��������Ҫ�޸ĵĻ�ԱID:");
    int id = atoi(idInput);

    // Check if ID is a 6-digit number
    if (!isSixDigitNumber(id)) {
        MessageBox(GetHWnd(), "����: ��ԱID������6λ������ɣ�", "Error", MB_OK);
        mysql_close(con);
        return;
    }

    // Check if the member exists
    char query[128];
    sprintf_s(query, "SELECT name, type, consumption FROM members WHERE id = %d", id);
    if (mysql_query(con, query)) {
        MessageBox(GetHWnd(), "��ѯʧ��.", "Error", MB_OK);
        mysql_close(con);
        return;
    }
    MYSQL_RES* res = mysql_store_result(con);
    if (mysql_num_rows(res) == 0) {
        MessageBox(GetHWnd(), "�޸�ID��Ա��Ϣ.", "Error", MB_OK);
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
            settextstyle(50, 0, "������κ");
            settextcolor(BLACK);      
            outtextxy(260, 60, "��Ա����ϵͳ"); 

            // Restore default style
            settextstyle(20, 0, "Arial");

            // Draw input boxes
            drawInputBox(nameBoxX1, nameBoxY1, nameBoxX2, nameBoxY2, nameInput);
            drawInputBox(typeBoxX1, typeBoxY1, typeBoxX2, typeBoxY2, typeInput);
            drawInputBox(consumptionBoxX1, consumptionBoxY1, consumptionBoxX2, consumptionBoxY2, consumptionInput); // Draw consumption input

            // Draw buttons
            drawButton(modifyButtonX1, modifyButtonY1, modifyButtonX2, modifyButtonY2, "�޸�", false);
            drawButton(returnButtonX1, returnButtonY1, returnButtonX2, returnButtonY2, "����", false);

            refreshRequired = false;
        }

        // Handle mouse events
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN) {
            // Click on the name input box
            if (msg.x > nameBoxX1 && msg.x < nameBoxRightX && msg.y > nameBoxY1 && msg.y < nameBoxBottomY) {
                InputBox(nameInput, 50, "�����µ�����:");
                refreshRequired = true;
            }
            // Click on the type input box
            else if (msg.x > typeBoxX1 && msg.x < typeBoxRightX && msg.y > typeBoxY1 && msg.y < typeBoxBottomY) {
                InputBox(typeInput, 10, "�������Ա���� (Gold/Silver):");
                refreshRequired = true;
            }
            // Click on the consumption input box
            else if (msg.x > consumptionBoxX1 && msg.x < consumptionBoxRightX && msg.y > consumptionBoxY1 && msg.y < consumptionBoxBottomY) {
                InputBox(consumptionInput, 20, "�������µ����ѽ��:");
                refreshRequired = true;
            }
            // Click on the modify button
            else if (msg.x > modifyButtonX1 && msg.x < modifyButtonX2 && msg.y > modifyButtonY1 && msg.y < modifyButtonY2) {
                // Validate the type input
                if (strcmp(typeInput, "Gold") != 0 && strcmp(typeInput, "Silver") != 0) {
                    MessageBox(GetHWnd(), "����: ��Ա���ͱ����� 'Gold' ���� 'Silver'.", "Error", MB_OK);
                }
                else {
                    // Validate the consumption value
                    double consumptionValue = atof(consumptionInput);
                    if (consumptionValue < 0) {
                        MessageBox(GetHWnd(), "���ѽ�����Ϊ����.", "Error", MB_OK);
                    }
                    else {
                        if (MessageBox(GetHWnd(), "ȷ���޸���", "��ʾ", MB_OKCANCEL) == IDOK) {
                            // Update the database
                            char updateQuery[256];
                            sprintf_s(updateQuery, "UPDATE members SET name = '%s', type = '%s', consumption = '%f' WHERE id = %d",
                                nameInput, typeInput, consumptionValue, id);
                            if (mysql_query(con, updateQuery)) {
                                MessageBox(GetHWnd(), "��Ա��Ϣ����ʧ��.", "Error", MB_OK);
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
                                    MessageBox(GetHWnd(), "���»�Ա����ʧ��.", "Error", MB_OK);
                                }
                                else {
                                    MessageBox(GetHWnd(), "��Ա��Ϣ���³ɹ�.", "Success", MB_OK);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            // ������ذ�ť
            else if (msg.x > returnButtonX1 && msg.x < returnButtonX2 && msg.y > returnButtonY1 && msg.y < returnButtonY2) {
                mysql_close(con);
                return;
            }
        }
    }
}

void countMembersByType() {
    // ��ʼ������
    int dropdownX1 = 300, dropdownY1 = 100, dropdownX2 = 500, dropdownY2 = 130;
    int dropdownOptionHeight = 30;
    std::string selectedType = "ѡ���Ա����";
    bool dropdownOpen = false;
    int page = 1, rowsPerPage = 6;
    int totalPages = 1;
    int totalMembers = 0;

    bool refreshRequired = true; // ���ڿ����Ƿ�ˢ����ʾ

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
        sprintf_s(pageInfo, "��ǰ��%dҳ / %dҳ", page, totalPages);
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
        // ��ӱ���
        settextstyle(50, 0, "������κ"); // ���ñ�������ʹ�С
        settextcolor(BLACK);          // ���ñ�����ɫ
        outtextxy(260, 30, "��Ա����ϵͳ"); // �������ݼ�λ��
        // �ָ�Ĭ����ʽ
        settextstyle(20, 0, "Arial");
        // ���Ʊ�����
        setlinecolor(BLACK);
        rectangle(startX, startY - 20, startX + idColumnWidth + nameColumnWidth, startY + rowHeight - 20);
        outtextxy(startX + (idColumnWidth - textwidth("��ԱID")) / 2, startY + (rowHeight - textheight("��ԱID")) / 2 - 20, "��ԱID");
        outtextxy(startX + idColumnWidth + (nameColumnWidth - textwidth("��Ա����")) / 2, startY + (rowHeight - textheight("��Ա����")) / 2 - 20, "��Ա����");
        line(startX, startY + rowHeight - 20, startX + idColumnWidth + nameColumnWidth, startY + rowHeight - 20);

        // ����������
        MYSQL_ROW row;
        int y = startY + rowHeight - 20;
        while ((row = mysql_fetch_row(res))) {
            rectangle(startX, y, startX + idColumnWidth + nameColumnWidth, y + rowHeight);
            line(startX + idColumnWidth, y, startX + idColumnWidth, y + rowHeight);

            outtextxy(startX + (idColumnWidth - textwidth(row[0])) / 2, y + (rowHeight - textheight(row[0])) / 2, row[0]);
            outtextxy(startX + idColumnWidth + (nameColumnWidth - textwidth(row[1])) / 2, y + (rowHeight - textheight(row[1])) / 2, row[1]);

            y += rowHeight;
        }

        // ����ͳ����Ϣ
        rectangle(startX, y, startX + idColumnWidth + nameColumnWidth, y + rowHeight);
        char totalDisplay[50];
        sprintf_s(totalDisplay, "�ܹ��� %s ��Ա: %d��", selectedType.c_str(), totalMembers);
        outtextxy(startX + (idColumnWidth + nameColumnWidth - textwidth(totalDisplay)) / 2, y + (rowHeight - textheight(totalDisplay)) / 2, totalDisplay);
    };

    while (true) {
        // ��������
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
                drawButton(650, 500, 750, 550, "����", false);
                if (page > 1) drawButton(150, 500, 250, 550, "��һҳ", false);
                if (page < totalPages) drawButton(300, 500, 400, 550, "��һҳ", false);
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
    // ���ƾ��б���
    int windowWidth = getwidth(); // ��ȡ���ڿ��
    int titleWidth = textwidth(title);
    int titleHeight = textheight(title);

    settextcolor(BLACK);
    setbkmode(TRANSPARENT);  // ���ñ���͸��
    settextstyle(50, 0, "������κ"); // ���ñ�������ʹ�С
    outtextxy((windowWidth - titleWidth) / 2 - 100, 20, title); // ������ʾ����
    settextstyle(20, 0, "Arial");
}

void showInstructionsAndExitIfNotConfirmed() {
    // ʹ�� MessageBox ��ʾʹ��˵��
    int msgResult = MessageBox(GetHWnd(), "��ӭʹ�û�Ա����ϵͳ��\n\n"
        "1. ��ӻ�Ա����������»�Ա��\n"
        "2. �鿴��Ա���鿴��ǰϵͳ�е����л�Ա��\n"
        "3. ɾ����Ա��ɾ��ָ���Ļ�Ա��\n"
        "4. �޸Ļ�Ա���޸Ļ�Ա�Ļ�����Ϣ��\n"
        "5. �����ͳ�ƻ�Ա������Ա���Ͳ鿴ͳ����Ϣ��\n\n"
        "���ȷ������ʹ�ã����ȡ���˳�ϵͳ��",
        "ϵͳʹ��˵��", MB_OKCANCEL | MB_ICONINFORMATION);

    // �ж��û���ѡ��
    if (msgResult == IDCANCEL) {
        // ����û����ȡ�������˳�����
        exit(0);
    }
}

void showMenu() {
    while (true) {
        // ʹ��˫���壬������˸
        BeginBatchDraw();

        cleardevice();
        putimage(0, 0, &image);
        // ���Ʊ���
        drawTitle("�򵥻�Ա����ϵͳ");

        MOUSEMSG msg = GetMouseMsg();

        // �жϰ�ť�Ƿ���ͣ
        bool addHovered = isButtonHovered(300, 100, 500, 150, msg);
        bool displayHovered = isButtonHovered(300, 160, 500, 210, msg);
        bool deleteHovered = isButtonHovered(300, 220, 500, 270, msg);
        bool modifyHovered = isButtonHovered(300, 280, 500, 330, msg);
        bool countHovered = isButtonHovered(300, 340, 500, 390, msg);
        bool exitHovered = isButtonHovered(300, 400, 500, 450, msg);

        // ���ư�ť����������ͣ״̬�ı���ɫ
        drawButton(300, 100, 500, 150, "��ӻ�Ա��Ϣ", addHovered);
        drawButton(300, 160, 500, 210, "��ѯ���л�Ա", displayHovered);
        drawButton(300, 220, 500, 270, "ɾ����Ա��Ϣ", deleteHovered);
        drawButton(300, 280, 500, 330, "�޸Ļ�Ա��Ϣ", modifyHovered);
        drawButton(300, 340, 500, 390, "��Ա����ͳ��", countHovered);
        drawButton(300, 400, 500, 450, "�˳�Ӧ�ó���", exitHovered);

        EndBatchDraw();  // ˫������������»���

        // ����ť����¼�
        if (isButtonClicked(300, 100, 500, 150, msg)) addMember();
        else if (isButtonClicked(300, 160, 500, 210, msg)) displayMembers();
        else if (isButtonClicked(300, 220, 500, 270, msg)) deleteMember();
        else if (isButtonClicked(300, 280, 500, 330, msg)) modifyMember();
        else if (isButtonClicked(300, 340, 500, 390, msg)) countMembersByType();
        else if (isButtonClicked(300, 400, 500, 450, msg)) {
            if (MessageBox(GetHWnd(), "ȷ���˳���ϵͳ��", "��ʾ", MB_OKCANCEL) == IDOK) {
                // ����û����"OK"�����˳�ѭ����ִ���˳�����
                break;
            }
            else {
                continue;
            }
        }
    }
}

int main() {
    // ����
    HWND hwndConsole = GetConsoleWindow();
    ShowWindow(hwndConsole, SW_HIDE);
    initgraph(800, 600);
    HWND hwnd = GetHWnd();
    // ʹ��˵��
    showInstructionsAndExitIfNotConfirmed();
    SetWindowText(hwnd, "�򵥻�Ա����ϵͳV1.0");
    // ����ͼ�걳��
    HICON hIcon = (HICON)LoadImage(NULL, APP_ICON, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    loadimage(&image, "./image.png", 800, 600);
    // �˵�
    showMenu();
    closegraph();
    DestroyIcon(hIcon);
    return 0;
}