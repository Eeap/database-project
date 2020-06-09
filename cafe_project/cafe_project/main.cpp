#include <iostream>
#include <mysql.h>
#include <iomanip>
#include <string>
#include <vector>
#pragma comment(lib, "libmysql.lib")
using namespace std;
int o_num;
const char *host = "localhost";
const char *user = "root";
const char *pw = "1234";
const char *db = "cafe";
int o_number = 0;

void menu(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result);
void order(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result);
bool menu_count_ck(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result, string item,int item_count);
void order_menu(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result, vector<string>& order, vector<int>& order_count);
int order_payment(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result, string& phone);
void managing(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result);
int main() {
	MYSQL Conn;
	MYSQL* Connptr = NULL;
	MYSQL_RES* Result= NULL;
	MYSQL_ROW Row;
	//connection �ʱ�ȭ & ����
	mysql_init(&Conn);
	Connptr = mysql_real_connect(&Conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (Connptr == NULL) {
		fprintf(stderr, "mysql connection error: %s\n", mysql_error(&Conn));
		return 1;
	}
	//���� ��¥�� ��ġ�ϴ� �� Ȯ��
	bool rev_date_ck = false;
	char* Query = "select rev_date from calculation where rev_date = curdate()";
	int stat = mysql_query(Connptr, Query);
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(0);
	}
	Result = mysql_store_result(Connptr);
	while ((Row = mysql_fetch_row(Result)) != NULL) {
		rev_date_ck = true;
	}
	if (!rev_date_ck) {
		Query = "insert into calculation(rev_date) values(curdate())";
		stat = mysql_query(Connptr, Query);
		if (stat != 0) {
			fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
			exit(0);
		}
	}
	//����
	while (1) {
		//������ ��¥ ǥ��
		Query = "select rev_date from calculation where rev_date = curdate()";
		stat = mysql_query(Connptr, Query);
		if (stat != 0) {
			fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
			exit(0);
		}
		Result = mysql_store_result(Connptr);
		while ((Row = mysql_fetch_row(Result)) != NULL) {
			printf("           *** %10s ***\n\n", Row[0]);
		}
		mysql_free_result(Result);
		printf("           * %10s    *\n", "DB_cafe");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "�޴��� ����");
		printf("           +----------------+\n");
		printf("           |%13s   |\n", "�ֹ��ϱ�");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "������ ���");
		printf("           +----------------+\n >>");
		string str;
		getline(cin, str);
		if (str == "�޴��� ����") {
			menu(Connptr, Conn, Result);
			printf("\n");
		}
		else if (str == "�ֹ��ϱ�") {
			order(Connptr, Conn, Result);
			printf("\n");
		}
		else if (str=="������ ���"){
			managing(Connptr, Conn, Result);
			printf("\n");
		}
		else { printf("  Error : ���⿡ �ִ� �͸� �Է� �����մϴ�.\n\n"); }
		
	}
	//���� ����
	mysql_close(Connptr);

	return 0;
}

void menu(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result) {
	MYSQL_ROW Row;
	printf("                     < �޴��� >                      \n");
	printf("+---------+-------------------+-------+----------+ \n");
	printf("|%8s | %18s| %6s| %9s| \n", "�����", "�޴� �̸�", "����", "����");
	printf("+---------+-------------------+-------+----------+\n");
	char *Query = "select m_count ,menu_name, price, category from menu order by category";
	int stat = mysql_query(Connptr, Query);
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(0);
	}
	Result = mysql_store_result(Connptr);
	while ((Row = mysql_fetch_row(Result)) != NULL) {
		printf("|%8s | %18s| %6s| %9s| \n", Row[0], Row[1], Row[2], Row[3]);
	}
	printf("+---------+-------------------+-------+----------+\n");
	mysql_free_result(Result);
}
bool menu_count_ck(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result,string item,int item_count){
	MYSQL_ROW Row;
	string query = "select m_count from menu where menu_name = '" + item + "'";
	bool check = false;
	int stat = mysql_query(Connptr, query.c_str());
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(0);
	}
	Result = mysql_store_result(Connptr);
	Row = mysql_fetch_row(Result);
	if (atoi(Row[0])-item_count >= 0) {
		check = true;
	}
	mysql_free_result(Result);
	return check;
}
void order_menu(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result, vector<string>& order, vector<int>& order_count){
	string o_no = to_string(o_number);

	for (int idx = 0; idx < order.size();idx++) {
		//order_menu�� �ֹ� ��ȣ�� ���� �޴� �ֱ�
		string query = "insert into order_menu values (" + o_no + ", '" + order[idx] + "', " + to_string(order_count[idx]) + ")";
		//cout << query << endl;
		int stat = mysql_query(Connptr, query.c_str());
		if (stat != 0) {
			fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
			exit(0);
		}
		//menu���� �ϳ��� ����
		query = "update menu set m_count=m_count - "+ to_string(order_count[idx]) +" where menu_name = '" + order[idx] + "'";
		stat = mysql_query(Connptr, query.c_str());
		if (stat != 0) {
			fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
			exit(0);
		}
	}

}
int order_payment(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result, string& phone) {
	MYSQL_ROW Row;
	string o_no = to_string(o_number);

	string query = "select m.price*o.o_count from menu m, order_menu o where o.menu_name = m.menu_name and o.o_no = '"+o_no+"'";
	int stat = mysql_query(Connptr, query.c_str());
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(0);
	}
	int sum = 0;
	Result = mysql_store_result(Connptr);
	while ((Row = mysql_fetch_row(Result)) != NULL) {
		sum += atoi(Row[0]);
	}
	mysql_free_result(Result);
	//���� ��� ���� üũ �� ���
	query = "select sign_up, g_savings from guests where phoneNum = '" + phone + "'";
	stat = mysql_query(Connptr, query.c_str());
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(0);
	}
	Result = mysql_store_result(Connptr);
	while ((Row = mysql_fetch_row(Result)) != NULL) {
		if (*Row[0] == 'Y') {
			cout << "���� ���� �ݾ��� " << Row[1] << "�� �Դϴ�.\n ����Ͻ� �ݾ��� �Է����ֽʽÿ�.\n(��, ���� �ݾ��� �Ѿ�� �ʰų� ���� �ݾ׿� �°� �Է����ֽð� ����Ͻ��� ���� ��쿣 0�� �Է����ֽʽÿ�.) \n >> ";
			int savings;
			cin >> savings;
			cin.ignore();
			sum -= savings;
		}
	}
	mysql_free_result(Result);

	//order_payment Ʃ�� ����
	query = "insert into order_payment values ('" + phone + "', " + "curdate()" + ", '" + o_no + "'," + to_string(sum) + ")";
	stat = mysql_query(Connptr, query.c_str());
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(0);
	}
	printf("\n�����Ͻ� �ݾ��� �� %d���Դϴ�\n", sum);
	return sum;

}
void order(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result) {
	string item,decision;
	int item_count;
	vector<string> order;
	vector<int> order_count;
	MYSQL_ROW Row;
	//ù�ֹ�
	while (1) {
		//�޴� �Է�
		printf("�޴� �Է� : ");
		getline(cin, item);
		printf("\n");
		//���� Ȯ��
		printf("���� �Է� : ");
		cin >> item_count;
		cin.ignore();
		printf("\n");
		//�޴� ������ 0�� �ƴ��� Ȯ��
		if (menu_count_ck(Connptr, Conn, Result, item,item_count)) {
			order.push_back(item);
			order_count.push_back(item_count);
			break;
		}
		else
			printf("����� �� ������ ��ǰ�Դϴ�. �ٸ� ��ǰ�� �ֹ����ֽʽÿ�.\n");
		
	}
	while (1) {
		printf("�߰� �ֹ��ϱ� or �����ϱ�\n >>");
		getline(cin,decision);
		printf("\n");
		//�߰� �ֹ�
		if (decision=="�߰� �ֹ��ϱ�")
		{ 
			printf("�޴� �Է� : ");
			getline(cin, item);
			printf("\n");
			//���� Ȯ��
			printf("���� �Է� : ");
			cin >> item_count;
			cin.ignore();
			printf("\n");
			//�޴� ������ 0�� �ƴ��� Ȯ��
			if (menu_count_ck(Connptr, Conn, Result, item,item_count)) {
				order.push_back(item);
				order_count.push_back(item_count);
			}
			else
				printf("����� �� ������ ��ǰ�Դϴ�. �ٸ� ��ǰ�� �ֹ����ֽʽÿ�.\n");
		}
		//���� �κ�
		else if (decision == "�����ϱ�") {
			printf("�̸� �Է�:");
			string name;
			getline(cin, name);
			printf("\n��ȭ��ȣ �Է�:");
			string phone;
			getline(cin,phone);
			printf("\n");
			o_number++;//�ֹ���ȣ ����
			//���� ��� �Ǻ� �κ�
			string query = "select sign_up from guests where phoneNum = '" + phone + "' and g_name = '"+name+"'";
			int stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(0);
			}
			bool check = false, sign_up = false;
			Result = mysql_store_result(Connptr);
			while ((Row = mysql_fetch_row(Result)) != NULL) {
				//printf(Row[0]);
				check = true;
				if (*Row[0] == 'Y') {
					sign_up = true;
				}//���Ե� ���
			}
			mysql_free_result(Result);
			//���� ���� �߰� 
			if (!check) {
				printf("\n���ο� ȸ���̽ʴϴ�, �����Ͻðڽ��ϱ�?( Y or N) >> ");
				string sign;
				getline(cin, sign);
				query = "insert into guests values ('" + phone + "', '"+name+ "', "+ '0' + ", '"+sign + "')";
				stat = mysql_query(Connptr, query.c_str());
				if (stat != 0) {
					fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
					exit(0);
				}
				check = true;
				if (sign == "Y")
					sign_up = true;
			}
			if (check){
				//�������� ��
				while (1) {
					//���Ե� ���
					if (sign_up) {
						//order_menu�� �ֹ� ��ȣ�� ���� �޴� �ֱ�
						order_menu(Connptr, Conn, Result, order,order_count);
						//���� �ݾ� ǥ�� �� order_payment ������ ������ ���� ����
						int sum = order_payment(Connptr,Conn,Result,  phone);
						//�����ϱ� �����ݾ��� 5%����
						query = "update guests set g_savings = g_savings +" + to_string(sum*0.05) + " where phoneNum = '" + phone + "' ";
						stat = mysql_query(Connptr, query.c_str());
						if (stat != 0) {
							fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
							exit(0);
						}
						cout << "\n"<<sum * 0.05 << "�� ��ŭ �����Ǽ̽��ϴ�." << "\n\n";
						printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
						break;// while�� Ż��
					}
					//���Ե��� ���� ���
					else {
						printf("ȸ�� �����Ͻðڽ��ϱ�?( Y or N) >> ");
						string sign;
						getline(cin, sign);
						if (sign == "Y") {
							sign_up = true;
							query = "update guests set sign_up = '" + sign + "' where phoneNum = '"+phone+"'";
							stat = mysql_query(Connptr, query.c_str());
							if (stat != 0) {
								fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
								exit(0);
							}
							continue;
						}
						//sing�� N�϶� ����ó��
						else {
							//order_menu�� �ֹ� ��ȣ�� ���� �޴� �ֱ�
							order_menu(Connptr, Conn, Result, order,order_count);
							//���� �ݾ� ǥ�� �� order_payment ������ ������ ���� ����
							int sum = order_payment(Connptr, Conn, Result, phone);
							printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
							break;
						}

					}
				}
			}

			break;
		}
		else { printf("�߸� �Է��ϼ̽��ϴ�."); }
	}
	
}
void managing(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result) {
	MYSQL_ROW Row;
	//�н����� ��ġ ���� Ȯ��
	string name, pswd;
	printf("\n�����ڸ��� �Է��Ͻÿ�.\n >>");
	getline(cin, name);
	printf("\n��й�ȣ�� �Է��Ͻÿ�.\n >>");
	getline(cin, pswd);
	string query = "select * from manager";
	int stat = mysql_query(Connptr, query.c_str());
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(100);
	}
	bool manager_check = false;
	Result = mysql_store_result(Connptr);
	while ((Row = mysql_fetch_row(Result)) != NULL) {
		if (Row[0] == name && Row[1] == pswd) {
			manager_check = true;
		}
	}
	mysql_free_result(Result);

	if (manager_check) {
		printf("\n ������ ��忡 �����ϼ̽��ϴ�. \n\n");
		// �޴� �߰� or ����
		printf("           +----------------+\n");
		printf("           |%15s |\n", "�޴� �߰�");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "�޴� ����");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "�����ϱ�");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "��� ä���");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "��¥ �� ����");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "�����ϱ�");
		printf("           +----------------+\n >>");
		// ���� ���
		string command;
		getline(cin, command);
		if (command == "�޴� �߰�") {
			string menu_name, category;
			int price, m_count;
			printf("\n�߰��� �޴� �̸��� �Է��Ͻÿ�,\n >>");
			getline(cin, menu_name);
			printf("\n������ �Է��Ͻÿ�,\n >>");
			cin >> price;
			cin.ignore();
			printf("\n������ �Է��Ͻÿ�,\n >>");
			getline(cin, category);
			printf("\n������� �Է��Ͻÿ�,\n >>");
			cin >> m_count;
			cin.ignore();
			//�޴� �߰�
			query = "insert into menu values('" + menu_name + "', " + to_string(price) + ",'" + category + "'," + to_string(m_count) +",'"+name+ "')";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(0);
			}
		}
		else if (command == "�޴� ����") {
			string menu_name;
			printf("\n������ �޴� �̸��� �Է��Ͻÿ�,\n >>");
			getline(cin, menu_name);
			query = "delete from menu where menu_name = '" + menu_name + "'";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(0);
			}
			printf("\n��û�Ͻ� �޴��� �����Ǿ����ϴ�.\n");
		}
		else if (command == "��� ä���") {
			string menu_name;
			int m_count;
			printf("\n��� ä�� �޴� �̸��� �Է��Ͻÿ�,\n >>");
			getline(cin, menu_name);
			printf("\n�߰��� ������ �Է��Ͻÿ�,\n >>");
			cin >> m_count;
			cin.ignore();
			query = "update menu set m_count = m_count +" + to_string(m_count) + " where menu_name = '" + menu_name + "'";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(0);
			}
			printf("\n��û�Ͻ� �޴� ����� ä�������ϴ�.\n");
		}
		else  if (command == "�����ϱ�") {
			string rev_date;
			int rev_sum=0;
			printf("\n������ ��¥�� �Է��Ͻÿ�,\n >>");
			getline(cin, rev_date);
			query = "select sum(payment) from order_payment group by o_date having o_date ='" + rev_date + "'";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(0);
			}
			Result = mysql_store_result(Connptr);
			while ((Row = mysql_fetch_row(Result)) != NULL) {
				if (Row[0] == NULL) { continue; }
				else {
					rev_sum = atoi(Row[0]);
				}
			}
			mysql_free_result(Result);
			query = "update calculation set revenue = revenue + "+to_string(rev_sum)+", m_name = '" + name + "' where rev_date = '" + rev_date + "'";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(0);
			}
			cout << rev_date << "�� ������ �Ϸ��߽��ϴ�. \n";
			//����� ���ÿ� �ֹ� ���� ����
			query = "delete from order_menu";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(0);
			}
			query = "delete from order_payment";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(0);
			}
			
		}
		else if(command == "�����ϱ�") {
			printf("\n���α׷��� �����մϴ�.\n");
			exit(0);
		}
		else if (command == "��¥ �� ����") {
			query = "select rev_date, revenue from calculation ";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(0);
			}
			Result = mysql_store_result(Connptr);
			printf("+------------+---------+\n");
			printf("|%12s|%9s|\n", "��¥", "����");
			printf("+------------+---------+\n");
			while ((Row = mysql_fetch_row(Result)) != NULL) {
				int temp = atoi(Row[1]);
				printf("|%12s|%9d|\n",Row[0],temp);
			}
			printf("+------------+---------+\n\n\n\n\n");
			mysql_free_result(Result);
		}
		else {
			printf("\n Error: �߸��� ����� �Է��ϼ̽��ϴ�.\n\n\n");
		}
	}
	else {
		printf("\n Error: ��й�ȣ �Ǵ� �����ڸ��� �߸� �Է��ϼ̽��ϴ�.\n\n\n\n");
	}
}