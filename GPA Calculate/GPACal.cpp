#include<iostream>
#include<bits/stdc++.h>
#include"sqlite/sqlite3.h"
#include <windows.h> // ���ڱ���ת������
using namespace std;

// ������������ GBK ������ַ���ת��Ϊ UTF-8
std::string GBKToUTF8(const std::string& gbkStr) {
	// �Ƚ� GBK ת��Ϊ���ַ���UTF-16��
	int wideLen = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, nullptr, 0);
	wchar_t* wideBuf = new wchar_t[wideLen];
	MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, wideBuf, wideLen);

	// �ٽ����ַ�ת��Ϊ UTF-8
	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideBuf, -1, nullptr, 0, nullptr, nullptr);
	char* utf8Buf = new char[utf8Len];
	WideCharToMultiByte(CP_UTF8, 0, wideBuf, -1, utf8Buf, utf8Len, nullptr, nullptr);

	std::string utf8Str(utf8Buf);
	delete[] wideBuf;
	delete[] utf8Buf;
	return utf8Str;
}

void addGrade(string stu_id) {
	
	string cour_name;
	cout << "������γ�����" << endl;
	cin >> cour_name;
	string cour_name_utf8 = GBKToUTF8(cour_name);

	int grade;
	cout << "������ÿγ̵ķ�����" << endl;
	while (true) {
		cin >> grade;
		if (grade >= 0 && grade <= 100) {
			break;
		}
		else {
			cout << "������������������" << endl;
		}
	}


	int credit;
	cout << "������ÿγ̵�ѧ�֣�" << endl;
	while (true) {
		cin >> credit;
		if (credit >= 0 && credit <= 5) {
			break;
		}
		else {
			cout << "������������������" << endl;
		}
	}

	double gp;
	cout << "������ÿγ̵ļ��㣺" << endl;
	while (true) {
		cin >> gp;
		if (gp >= 0 && gp <= 5) {
			break;
		}
		else {
			cout << "������������������" << endl;
		}
	}

	int type;
	cout << "������ÿγ̵����ͣ�1���޻�0�Ǳ��ޣ���" << endl;
	while (true) {
		cin >> type;
		if (type==0||type==1) {
			break;
		}
		else {
			cout << "������������������" << endl;
		}
	}

	

	

	//�����ݿ�
	sqlite3* db;
	char* errmeg = 0;
	int rc;
	rc = sqlite3_open("resource/grade.db", &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}

	const char* sql;
	sql = u8"INSERT INTO c_grade(stu_Id,courseName,Grade,Credit,Gp,Type)VALUES(?, ?, ? , ? , ? , ? ); ";
	sqlite3_stmt* stmt = nullptr;

	// ׼�� SQL ���
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		cerr << "׼���������ʧ��: " << sqlite3_errmsg(db) << endl;
		return;
	}

	// �󶨲�����ע�ⶼ���ַ�����
	sqlite3_bind_text(stmt, 1, stu_id.c_str(),-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, cour_name_utf8.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 3, grade);
	sqlite3_bind_int(stmt, 4, credit);
	sqlite3_bind_double(stmt, 5, gp);
	sqlite3_bind_int(stmt, 6, type);
	// ִ�в���
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		cerr << "����ʧ��: " << sqlite3_errmsg(db) << endl;
	}

	sqlite3_finalize(stmt); // �ͷ���Դ
	sqlite3_close(db);
	system("pause");
	system("cls");
}
void searchGrade(string stu_id) {
	sqlite3* db;
	char* errMsg = nullptr;
	int rc;
	double gpa = 0.0;

	// �����ݿ�
	rc = sqlite3_open("resource/grade.db", &db);
	if (rc) {
		std::cerr << "�޷������ݿ�:" << sqlite3_errmsg(db) << std::endl;
		return;
	}

	const char* sql;
	sql= "SELECT SUM(Gp * Credit), SUM(Credit) FROM c_grade WHERE Type=1 AND stu_Id = ? GROUP BY stu_Id;";

	sqlite3_stmt* stmt=nullptr;
	// ׼�� SQL ���
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "׼�����ʧ��:" << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	// �󶨲�����ѧ�� ID
	sqlite3_bind_text(stmt, 1, stu_id.c_str(), -1, SQLITE_STATIC);

	// ִ�в�ѯ��������
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		// ��ȡ��ѯ�����ע�⣺SUM ���ܷ��� NULL����Ҫ��ȫ����
		double totalGpCredit = 0.0;
		if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
			totalGpCredit = sqlite3_column_double(stmt, 0); // ֱ������ֵ�ӿڸ���ȫ
		}

		int totalCredit = 0;
		if (sqlite3_column_type(stmt, 1) != SQLITE_NULL) {
			totalCredit = sqlite3_column_int(stmt, 1); // ֱ������ֵ�ӿڸ���ȫ
		}

		// ���� GPA
		if (totalCredit > 0) {
			gpa = floor((totalGpCredit / totalCredit) * 100) / 100;
		}
		// ������
		cout << endl << endl << endl;
		std::cout << "���޿� GPA:" << gpa << std::endl;
	}
	else if (sqlite3_step(stmt) == SQLITE_DONE) {
		std::cout << "δ�ҵ�ѧ��" << stu_id << "�ı��޿μ�¼" << std::endl;
	}
	else {
		std::cerr << "��ѯִ��ʧ��:" << sqlite3_errmsg(db) << std::endl;
	}

	// ������Դ
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	gpa = 0;
	// �����ݿ�
	rc = sqlite3_open("./resource/grade.db", &db);
	if (rc) {
		std::cerr << "�޷������ݿ�:" << sqlite3_errmsg(db) << std::endl;
		return;
	}

	sql = "SELECT SUM(Gp * Credit), SUM(Credit) FROM c_grade WHERE stu_Id = ? GROUP BY stu_Id;";

	stmt = nullptr;
	// ׼�� SQL ���
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "׼�����ʧ��:" << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	// �󶨲�����ѧ�� ID
	sqlite3_bind_text(stmt, 1, stu_id.c_str(), -1, SQLITE_STATIC);

	// ִ�в�ѯ��������
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		// ��ȡ��ѯ�����ע�⣺SUM ���ܷ��� NULL����Ҫ��ȫ����
		double totalGpCredit = 0.0;
		if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
			totalGpCredit = sqlite3_column_double(stmt, 0); // ֱ������ֵ�ӿڸ���ȫ
		}

		int totalCredit = 0;
		if (sqlite3_column_type(stmt, 1) != SQLITE_NULL) {
			totalCredit = sqlite3_column_int(stmt, 1); // ֱ������ֵ�ӿڸ���ȫ
		}

		// ���� GPA
		if (totalCredit > 0) {
			gpa = floor((totalGpCredit / totalCredit) * 100) / 100;
		}

		// ������
		std::cout << "ȫ���γ� GPA:" << gpa << std::endl;
		cout << endl;
	}
	else if (sqlite3_step(stmt) == SQLITE_DONE) {
		std::cout << "δ�ҵ�ѧ��" << stu_id << "�ı��޿μ�¼" << std::endl;
	}
	else {
		std::cerr << "��ѯִ��ʧ��:" << sqlite3_errmsg(db) << std::endl;
	}

	// ������Դ
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}
void opermenu(string stu_id) {
	cout << "��ӭѧ���û���" << stu_id << "��¼" << endl;
	cout << "\t\t ---------------------------------------\n";
	cout << "\t\t|                                       |\n";
	cout << "\t\t|              1, ��ӿ�Ŀ�ɼ�          |\n";
	cout << "\t\t|                                       |\n";
	cout << "\t\t|              2, ��ѯGPA               |\n";
	cout << "\t\t|                                       |\n";
	cout << "\t\t|              0, �˳�                  |\n";
	cout << "\t\t|                                       |\n";
	cout << "\t\t ---------------------------------------\n";
	int select;
	while (1) {
		cout << "����������ѡ��" << endl;
		cin >> select;
		if (select == 1) {
			addGrade(stu_id);
			system("pause");
		}
		else if(select==2) {
			searchGrade(stu_id);
			system("pause");
		}
		else {
			break;
		}
	}
}
int main(){
	string  stu_id;
	while (true) {
		cout << "********************��ӭʹ��GPA������************************" << endl;
		cout << endl;
		cout << "����������ѧ��" << endl;
		cin >> stu_id;

		cout << "��¼�ɹ�" << endl;
		system("pause");
		system("cls");
		opermenu(stu_id);
		system("pause");
		system("cls");

	}
   return 0;
}