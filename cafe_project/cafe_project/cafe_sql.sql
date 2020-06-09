
#메뉴 테이블
create table menu(
  menu_name varchar(18) not null,
  price int(4) not null,
  category varchar(7) not null,
  m_count int(2) default 0,
  constraint category_ck check (category In ('dessert','juice','coffee','latte','tea')),
  constraint price_ck check (price >= 0 ),
  constraint m_count_ck check (m_count >= 0 ),
  PRIMARY KEY(menu_name));
#메뉴 추가
insert into menu values('espresso',2500,'coffee',10);
insert into menu values('americano',3000,'coffee',10);
insert into menu values('cappuccino',4000,'coffee',10);
insert into menu values('caffe mocha',4500,'coffee',10);
insert into menu values('caramel macchiato',4500,'coffee',10);
insert into menu values('affogato',5000,'coffee',10);
insert into menu values('cafe latte',4000,'coffee',10);
insert into menu values('vanilla latte',4500,'coffee',10);
insert into menu values('chocolate latte',4000,'latte',10);
insert into menu values('green tea latte',4000,'latte',10);
insert into menu values('lemon tea',3500,'tea',10);
insert into menu values('green tea',3500,'tea',10);
insert into menu values('strawberry juice',4500,'juice',10);
insert into menu values('kiwi juice',4500,'juice',10);
insert into menu values('pineapple juice',4500,'juice',10);
insert into menu values('honey bread',4500,'dessert',10);
insert into menu values('cookie',3000,'dessert',10);
insert into menu values('cake',3000,'dessert',10);

#손님 테이블
create table guests(
	phoneNum char(11) unique,
	g_name varchar(16) not null,
	g_savings int(5),
	sign_up char(1) default 'N',
	constraint sign_ck check (sign_up in ('Y','N')),
	primary key(phoneNum));

#관리자 테이블
create table manager(
	m_name varchar(16) not null,
    pswd varchar(10) not null,
    primary key(m_name));

#주문 결제 테이블
create table order_payment(
  phoneNum char(11) unique,
  o_date datetime not null,
  o_no int(10) not null,
  payment int(8) check (payment >= 0),
  constraint pk_orderPayment primary key(phoneNum,o_date,o_no));

#주문 메뉴 테이블
create table order_menu(
  o_no int(10) not null,
  menu_name varchar(18) not null,
  o_count tinyint(2) default 1,
  constraint pk_orderMoney primary key(o_no,menu_name));

#정산 테이블
create table calculation(
	rev_date datetime not null,
    revenue int(16) default 0 check (revenue >= 0) not null,
    primary key(rev_date));
	

  
