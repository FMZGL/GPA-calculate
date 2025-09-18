#include<iostream>
#include<bits/stdc++.h>
#include"sqlite/sqlite3.h"
#include <windows.h> // 用于编码转换函数
using namespace std;

// 辅助函数：将 GBK 编码的字符串转换为 UTF-8
std::string GBKToUTF8(const std::string& gbkStr) {
	// 先将 GBK 转换为宽字符（UTF-16）
	int wideLen = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, nullptr, 0);
	wchar_t* wideBuf = new wchar_t[wideLen];
	MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, wideBuf, wideLen);

	// 再将宽字符转换为 UTF-8
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
	cout << "请输入课程名：" << endl;
	cin >> cour_name;
	string cour_name_utf8 = GBKToUTF8(cour_name);

	int grade;
	cout << "请输入该课程的分数：" << endl;
	while (true) {
		cin >> grade;
		if (grade >= 0 && grade <= 100) {
			break;
		}
		else {
			cout << "输入有误，请重新输入" << endl;
		}
	}


	int credit;
	cout << "请输入该课程的学分：" << endl;
	while (true) {
		cin >> credit;
		if (credit >= 0 && credit <= 5) {
			break;
		}
		else {
			cout << "输入有误，请重新输入" << endl;
		}
	}

	double gp;
	cout << "请输入该课程的绩点：" << endl;
	while (true) {
		cin >> gp;
		if (gp >= 0 && gp <= 5) {
			break;
		}
		else {
			cout << "输入有误，请重新输入" << endl;
		}
	}

	int type;
	cout << "请输入该课程的类型（1必修或0非必修）：" << endl;
	while (true) {
		cin >> type;
		if (type==0||type==1) {
			break;
		}
		else {
			cout << "输入有误，请重新输入" << endl;
		}
	}

	

	

	//打开数据库
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

	// 准备 SQL 语句
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		cerr << "准备插入语句失败: " << sqlite3_errmsg(db) << endl;
		return;
	}

	// 绑定参数（注意都是字符串）
	sqlite3_bind_text(stmt, 1, stu_id.c_str(),-1,SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, cour_name_utf8.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 3, grade);
	sqlite3_bind_int(stmt, 4, credit);
	sqlite3_bind_double(stmt, 5, gp);
	sqlite3_bind_int(stmt, 6, type);
	// 执行插入
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		cerr << "插入失败: " << sqlite3_errmsg(db) << endl;
	}

	sqlite3_finalize(stmt); // 释放资源
	sqlite3_close(db);
	system("pause");
	system("cls");
}
void searchGrade(string stu_id) {
	sqlite3* db;
	char* errMsg = nullptr;
	int rc;
	double gpa = 0.0;

	// 打开数据库
	rc = sqlite3_open("resource/grade.db", &db);
	if (rc) {
		std::cerr << "无法打开数据库:" << sqlite3_errmsg(db) << std::endl;
		return;
	}

	const char* sql;
	sql= "SELECT SUM(Gp * Credit), SUM(Credit) FROM c_grade WHERE Type=1 AND stu_Id = ? GROUP BY stu_Id;";

	sqlite3_stmt* stmt=nullptr;
	// 准备 SQL 语句
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "准备语句失败:" << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	// 绑定参数：学生 ID
	sqlite3_bind_text(stmt, 1, stu_id.c_str(), -1, SQLITE_STATIC);

	// 执行查询并处理结果
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		// 提取查询结果（注意：SUM 可能返回 NULL，需要安全处理）
		double totalGpCredit = 0.0;
		if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
			totalGpCredit = sqlite3_column_double(stmt, 0); // 直接用数值接口更安全
		}

		int totalCredit = 0;
		if (sqlite3_column_type(stmt, 1) != SQLITE_NULL) {
			totalCredit = sqlite3_column_int(stmt, 1); // 直接用数值接口更安全
		}

		// 计算 GPA
		if (totalCredit > 0) {
			gpa = floor((totalGpCredit / totalCredit) * 100) / 100;
		}
		// 输出结果
		cout << endl << endl << endl;
		std::cout << "必修课 GPA:" << gpa << std::endl;
	}
	else if (sqlite3_step(stmt) == SQLITE_DONE) {
		std::cout << "未找到学生" << stu_id << "的必修课记录" << std::endl;
	}
	else {
		std::cerr << "查询执行失败:" << sqlite3_errmsg(db) << std::endl;
	}

	// 清理资源
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	gpa = 0;
	// 打开数据库
	rc = sqlite3_open("./resource/grade.db", &db);
	if (rc) {
		std::cerr << "无法打开数据库:" << sqlite3_errmsg(db) << std::endl;
		return;
	}

	sql = "SELECT SUM(Gp * Credit), SUM(Credit) FROM c_grade WHERE stu_Id = ? GROUP BY stu_Id;";

	stmt = nullptr;
	// 准备 SQL 语句
	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::cerr << "准备语句失败:" << sqlite3_errmsg(db) << std::endl;
		sqlite3_close(db);
		return;
	}

	// 绑定参数：学生 ID
	sqlite3_bind_text(stmt, 1, stu_id.c_str(), -1, SQLITE_STATIC);

	// 执行查询并处理结果
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		// 提取查询结果（注意：SUM 可能返回 NULL，需要安全处理）
		double totalGpCredit = 0.0;
		if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
			totalGpCredit = sqlite3_column_double(stmt, 0); // 直接用数值接口更安全
		}

		int totalCredit = 0;
		if (sqlite3_column_type(stmt, 1) != SQLITE_NULL) {
			totalCredit = sqlite3_column_int(stmt, 1); // 直接用数值接口更安全
		}

		// 计算 GPA
		if (totalCredit > 0) {
			gpa = floor((totalGpCredit / totalCredit) * 100) / 100;
		}

		// 输出结果
		std::cout << "全部课程 GPA:" << gpa << std::endl;
		cout << endl;
	}
	else if (sqlite3_step(stmt) == SQLITE_DONE) {
		std::cout << "未找到学生" << stu_id << "的必修课记录" << std::endl;
	}
	else {
		std::cerr << "查询执行失败:" << sqlite3_errmsg(db) << std::endl;
	}

	// 清理资源
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}
void opermenu(string stu_id) {
	cout << "欢迎学生用户：" << stu_id << "登录" << endl;
	cout << "\t\t ---------------------------------------\n";
	cout << "\t\t|                                       |\n";
	cout << "\t\t|              1, 添加科目成绩          |\n";
	cout << "\t\t|                                       |\n";
	cout << "\t\t|              2, 查询GPA               |\n";
	cout << "\t\t|                                       |\n";
	cout << "\t\t|              0, 退出                  |\n";
	cout << "\t\t|                                       |\n";
	cout << "\t\t ---------------------------------------\n";
	int select;
	while (1) {
		cout << "请输入您的选择" << endl;
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
		cout << "********************欢迎使用GPA计算器************************" << endl;
		cout << endl;
		cout << "请输入您的学号" << endl;
		cin >> stu_id;

		cout << "登录成功" << endl;
		system("pause");
		system("cls");
		opermenu(stu_id);
		system("pause");
		system("cls");

	}
   return 0;
}