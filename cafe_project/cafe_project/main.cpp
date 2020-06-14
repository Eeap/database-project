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
	//connection 초기화 & 연결
	mysql_init(&Conn);
	Connptr = mysql_real_connect(&Conn, host, user, pw, db, 3306, (const char*)NULL, 0);
	if (Connptr == NULL) {
		fprintf(stderr, "mysql connection error: %s\n", mysql_error(&Conn));
		return 1;
	}
	//오늘 날짜와 일치하는 지 확인
	bool rev_date_ck = false;
	char* Query = "select rev_date from calculation where rev_date = curdate()";
	int stat = mysql_query(Connptr, Query);
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(100);
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
			exit(100);
		}
	}
	//시작
	while (1) {
		//오늘의 날짜 표시
		Query = "select rev_date from calculation where rev_date = curdate()";
		stat = mysql_query(Connptr, Query);
		if (stat != 0) {
			fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
			exit(100);
		}
		Result = mysql_store_result(Connptr);
		while ((Row = mysql_fetch_row(Result)) != NULL) {
			printf("           *** %10s ***\n\n", Row[0]);
		}
		mysql_free_result(Result);
		printf("           * %10s    *\n", "DB_cafe");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "메뉴판 보기");
		printf("           +----------------+\n");
		printf("           |%13s   |\n", "주문하기");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "관리자 모드");
		printf("           +----------------+\n >>");
		string str;
		getline(cin, str);
		if (str == "메뉴판 보기") {
			menu(Connptr, Conn, Result);
			printf("\n");
		}
		else if (str == "주문하기") {
			order(Connptr, Conn, Result);
			printf("\n");
		}
		else if (str=="관리자 모드"){
			managing(Connptr, Conn, Result);
			printf("\n");
		}
		else { printf("  Error : 보기에 있는 것만 입력 가능합니다.\n\n"); }
		
	}
	//연결 종료
	mysql_close(Connptr);

	return 0;
}

void menu(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result) {
	MYSQL_ROW Row;
	printf("                     < 메뉴판 >                      \n");
	printf("+---------+-------------------+-------+----------+ \n");
	printf("|%8s | %18s| %6s| %9s| \n", "재고량", "메뉴 이름", "가격", "종류");
	printf("+---------+-------------------+-------+----------+\n");
	char *Query = "select m_count ,menu_name, price, category from menu order by category";
	int stat = mysql_query(Connptr, Query);
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(100);
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
		exit(100);
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
		//order_menu에 주문 번호에 대한 메뉴 넣기
		string query = "insert into order_menu values (" + o_no + ", '" + order[idx] + "', " + to_string(order_count[idx]) + ")";
		//cout << query << endl;
		int stat = mysql_query(Connptr, query.c_str());
		if (stat != 0) {
			fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
			exit(100);
		}
		//menu에서 하나씩 빼기
		query = "update menu set m_count=m_count - "+ to_string(order_count[idx]) +" where menu_name = '" + order[idx] + "'";
		stat = mysql_query(Connptr, query.c_str());
		if (stat != 0) {
			fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
			exit(100);
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
		exit(100);
	}
	int sum = 0;
	Result = mysql_store_result(Connptr);
	while ((Row = mysql_fetch_row(Result)) != NULL) {
		sum += atoi(Row[0]);
	}
	mysql_free_result(Result);
	//적립 사용 여부 체크 후 사용
	query = "select sign_up, g_savings from guests where phoneNum = '" + phone + "'";
	stat = mysql_query(Connptr, query.c_str());
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(100);
	}
	Result = mysql_store_result(Connptr);
	while ((Row = mysql_fetch_row(Result)) != NULL) {
		if (*Row[0] == 'Y') {
			cout << "현재 적립 금액은 " << Row[1] << "원 입니다.\n 사용하실 금액을 입력해주십시오.\n(단, 결제 금액을 넘어가지 않거나 적립 금액에 맞게 입력해주시고 사용하시지 않을 경우엔 0을 입력해주십시오.) \n >> ";
			int savings;
			cin >> savings;
			cin.ignore();
			sum -= savings;
		}
	}
	mysql_free_result(Result);

	//order_payment 튜플 삽입
	query = "insert into order_payment values ('" + phone + "', " + "curdate()" + ", '" + o_no + "'," + to_string(sum) + ")";
	stat = mysql_query(Connptr, query.c_str());
	if (stat != 0) {
		fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
		exit(100);
	}
	printf("\n결제하실 금액은 총 %d원입니다\n", sum);
	return sum;

}
void order(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result) {
	string item,decision;
	int item_count;
	vector<string> order;
	vector<int> order_count;
	MYSQL_ROW Row;
	//첫주문
	while (1) {
		//메뉴 입력
		printf("메뉴 입력 : ");
		getline(cin, item);
		printf("\n");
		//수량 확인
		printf("수량 입력 : ");
		cin >> item_count;
		cin.ignore();
		printf("\n");
		//메뉴 수량이 0이 아닌지 확인
		if (menu_count_ck(Connptr, Conn, Result, item,item_count)) {
			order.push_back(item);
			order_count.push_back(item_count);
			break;
		}
		else
			printf("재고가 다 떨어진 상품입니다. 다른 제품을 주문해주십시오.\n");
		
	}
	while (1) {
		printf("추가 주문하기 or 결제하기\n >>");
		getline(cin,decision);
		printf("\n");
		//추가 주문
		if (decision=="추가 주문하기")
		{ 
			printf("메뉴 입력 : ");
			getline(cin, item);
			printf("\n");
			//수량 확인
			printf("수량 입력 : ");
			cin >> item_count;
			cin.ignore();
			printf("\n");
			//메뉴 수량이 0이 아닌지 확인
			if (menu_count_ck(Connptr, Conn, Result, item,item_count)) {
				order.push_back(item);
				order_count.push_back(item_count);
			}
			else
				printf("재고가 다 떨어진 상품입니다. 다른 제품을 주문해주십시오.\n");
		}
		//결제 부분
		else if (decision == "결제하기") {
			printf("이름 입력:");
			string name;
			getline(cin, name);
			printf("\n전화번호 입력:");
			string phone;
			getline(cin,phone);
			printf("\n");
			o_number++;//주문번호 증가
			//가입 대상 판별 부분
			string query = "select sign_up from guests where phoneNum = '" + phone + "' and g_name = '"+name+"'";
			int stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(100);
			}
			bool check = false, sign_up = false;
			Result = mysql_store_result(Connptr);
			while ((Row = mysql_fetch_row(Result)) != NULL) {
				//printf(Row[0]);
				check = true;
				if (*Row[0] == 'Y') {
					sign_up = true;
				}//가입된 대상
			}
			mysql_free_result(Result);
			//고객 정보 추가 
			if (!check) {
				printf("\n새로운 회원이십니다, 가입하시겠습니까?( Y or N) >> ");
				string sign;
				getline(cin, sign);
				query = "insert into guests values ('" + phone + "', '"+name+ "', "+ '0' + ", '"+sign + "')";
				stat = mysql_query(Connptr, query.c_str());
				if (stat != 0) {
					fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
					exit(100);
				}
				check = true;
				if (sign == "Y")
					sign_up = true;
			}
			if (check){
				//가입하지 않
				while (1) {
					//가입된 대상
					if (sign_up) {
						//order_menu에 주문 번호에 대한 메뉴 넣기
						order_menu(Connptr, Conn, Result, order,order_count);
						//결제 금액 표시 및 order_payment 데이터 삽입을 위한 구현
						int sum = order_payment(Connptr,Conn,Result,  phone);
						//적립하기 결제금액의 5%적립
						query = "update guests set g_savings = g_savings +" + to_string(sum*0.05) + " where phoneNum = '" + phone + "' ";
						stat = mysql_query(Connptr, query.c_str());
						if (stat != 0) {
							fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
							exit(100);
						}
						cout << "\n"<<sum * 0.05 << "원 만큼 적립되셨습니다." << "\n\n";
						printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
						break;// while문 탈출
					}
					//가입되지 않은 대상
					else {
						printf("회원 가입하시겠습니까?( Y or N) >> ");
						string sign;
						getline(cin, sign);
						if (sign == "Y") {
							sign_up = true;
							query = "update guests set sign_up = '" + sign + "' where phoneNum = '"+phone+"'";
							stat = mysql_query(Connptr, query.c_str());
							if (stat != 0) {
								fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
								exit(100);
							}
							continue;
						}
						//sing이 N일때 고객처리
						else {
							//order_menu에 주문 번호에 대한 메뉴 넣기
							order_menu(Connptr, Conn, Result, order,order_count);
							//결제 금액 표시 및 order_payment 데이터 삽입을 위한 구현
							int sum = order_payment(Connptr, Conn, Result, phone);
							printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
							break;
						}

					}
				}
			}

			break;
		}
		else { printf("잘못 입력하셨습니다."); }
	}
	
}
void managing(MYSQL* Connptr, MYSQL &Conn, MYSQL_RES* Result) {
	MYSQL_ROW Row;
	//패스워드 일치 여부 확인
	string name, pswd;
	printf("\n관리자명을 입력하시오.\n >>");
	getline(cin, name);
	printf("\n비밀번호를 입력하시오.\n >>");
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
		printf("\n 관리자 모드에 접근하셨습니다. \n\n");
		// 메뉴 추가 or 삭제
		printf("           +----------------+\n");
		printf("           |%15s |\n", "메뉴 추가");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "메뉴 삭제");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "정산하기");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "재고 채우기");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "날짜 별 수익");
		printf("           +----------------+\n");
		printf("           |%15s |\n", "종료하기");
		printf("           +----------------+\n >>");
		// 정산 기능
		string command;
		getline(cin, command);
		if (command == "메뉴 추가") {
			string menu_name, category;
			int price, m_count;
			printf("\n추가할 메뉴 이름을 입력하시오,\n >>");
			getline(cin, menu_name);
			printf("\n가격을 입력하시오,\n >>");
			cin >> price;
			cin.ignore();
			printf("\n종류를 입력하시오,\n >>");
			getline(cin, category);
			printf("\n재고량을 입력하시오,\n >>");
			cin >> m_count;
			cin.ignore();
			//메뉴 추가
			query = "insert into menu values('" + menu_name + "', " + to_string(price) + ",'" + category + "'," + to_string(m_count) +",'"+name+ "')";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(100);
			}
		}
		else if (command == "메뉴 삭제") {
			string menu_name;
			printf("\n삭제할 메뉴 이름을 입력하시오,\n >>");
			getline(cin, menu_name);
			query = "delete from menu where menu_name = '" + menu_name + "'";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(100);
			}
			printf("\n요청하신 메뉴가 삭제되었습니다.\n");
		}
		else if (command == "재고 채우기") {
			string menu_name;
			int m_count;
			printf("\n재고 채울 메뉴 이름을 입력하시오,\n >>");
			getline(cin, menu_name);
			printf("\n추가할 수량을 입력하시오,\n >>");
			cin >> m_count;
			cin.ignore();
			query = "update menu set m_count = m_count +" + to_string(m_count) + " where menu_name = '" + menu_name + "'";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(100);
			}
			printf("\n요청하신 메뉴 재고가 채워졌습니다.\n");
		}
		else  if (command == "정산하기") {
			string rev_date;
			int rev_sum=0;
			printf("\n정산할 날짜를 입력하시오,\n >>");
			getline(cin, rev_date);
			//NVL함수 없으므로 ifnull씀
			query = "select ifnull(sum(payment),0) from order_payment group by o_date having o_date ='" + rev_date + "'";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(100);
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
				exit(100);
			}
			cout << rev_date << "의 정산을 완료했습니다. \n";
			//정산과 동시에 주문 내역 삭제
			query = "delete from order_menu";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(100);
			}
			query = "delete from order_payment";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(100);
			}
			
		}
		else if(command == "종료하기") {
			printf("\n프로그램을 종료합니다.\n");
			exit(100);
		}
		else if (command == "날짜 별 수익") {
			query = "select rev_date, revenue from calculation ";
			stat = mysql_query(Connptr, query.c_str());
			if (stat != 0) {
				fprintf(stderr, "mysql query error: %s\n", mysql_error(&Conn));
				exit(100);
			}
			Result = mysql_store_result(Connptr);
			printf("+------------+---------+\n");
			printf("|%12s|%9s|\n", "날짜", "수익");
			printf("+------------+---------+\n");
			while ((Row = mysql_fetch_row(Result)) != NULL) {
				int temp = atoi(Row[1]);
				printf("|%12s|%9d|\n",Row[0],temp);
			}
			printf("+------------+---------+\n\n\n\n\n");
			mysql_free_result(Result);
		}
		else {
			printf("\n Error: 잘못된 기능을 입력하셨습니다.\n\n\n");
		}
	}
	else {
		printf("\n Error: 비밀번호 또는 관리자명을 잘못 입력하셨습니다.\n\n\n\n");
	}
}