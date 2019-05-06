#include <Winsock2.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#define DEFAULT_PORT 5055
#define DATA_BUFFER 1024
using namespace std;
class client
{
public:
    bool Startup();
    void SetServerAddress();
	int GeneratePort();               //随机生成显示器端口号
	bool Getonlinelist();             //获得在线的用户名
	void work();                      //发送器的主函数
	void succ();
private:
    WSADATA wsaData;
    SOCKET sClient;
    struct sockaddr_in ser;
    int ser_length = sizeof(ser);
    struct sockaddr_in com;
    int com_len = sizeof(com);
    char recv_buf[DATA_BUFFER];
    int receiver_port;
    vector<string> onlinelist;
    int isend,irecv;
};
string tostring(int v)
{
    string k="";
    while(v)
    {   k +='0'+(v%10); v/=10;}
    string t="";
    for(int i=k.size()-1;i>=0;i--) t+=k[i];
    return t;
}
bool client::Startup()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Failed to load Winsock." << endl;
		return false;
	}
	sClient = socket(AF_INET, SOCK_DGRAM, 0);
	if (sClient == INVALID_SOCKET)
	{
		cout << "socket()Failed:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
void client::SetServerAddress()
{
	cout << "请输入ip地址:";
	string iptemp;
	cin >> iptemp;
	char *ip = new char[iptemp.length() + 1];
	strcpy(ip, iptemp.c_str());
	//建立服务器端地址
	ser.sin_family = AF_INET;
	ser.sin_port = htons(DEFAULT_PORT);
	ser.sin_addr.s_addr = inet_addr(ip);
}
int client::GeneratePort()
{
	srand((unsigned)time(NULL));
	int x = 1024 + rand() % (5000 - 1024);
	return x;
}
void client::succ()
{
	char send_buf[DATA_BUFFER];
    while(1)
    {
        gets(send_buf);
        isend=sendto(sClient,send_buf,sizeof(send_buf),0,(struct sockaddr*)&ser,ser_length);
        if(isend==SOCKET_ERROR)
    	{
         printf("sendto()Failed:%d\n",WSAGetLastError());
       	 return ;
   		 }
         else if(isend==0) return ;

        if(send_buf[0]=='#') break;
    }
}
void client::work()
{
    while(true)
    {
        memset(recv_buf,0,sizeof(recv_buf));
        system("cls");
        cout << "****************************************" << endl;
		cout << "*                                      *" << endl;
		cout << "*       1.登录  2.注册  3.退出         *" << endl;
		cout << "*                                      *" << endl;
		cout << "****************************************" << endl;
        string choice;
        getline(cin,choice);
        if(choice=="1")
        {
            system("cls");
            cout << "请输入用户名:";
			string username;
			getline(cin, username);
			cout << "请输入密码:";
			string password;
			getline(cin, password);
            receiver_port = GeneratePort();
            ofstream out("port.txt");
			out << receiver_port << "\n" << username;
			out.close();
            string init_infortemp = "L" + username + "#" + password + "#" + tostring(receiver_port);
            char *init_infor = new char[init_infortemp.length() + 1];
			strcpy(init_infor, init_infortemp.c_str());
			//向服务器验证用户信息
			isend = sendto(sClient, init_infor, init_infortemp.length() + 1, 0, (struct sockaddr*)&ser, ser_length);
			//接收服务器回应的消息
			irecv = recvfrom(sClient, recv_buf, sizeof(recv_buf), 0, (SOCKADDR*)&com, &com_len);
            if (irecv == SOCKET_ERROR)
			{
				cout << "recvfrom() Failed:" << GetLastError() << endl;
				cout << "未收到服务器的响应，登录失败,请输入Y返回首页:";
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "未收到服务器的响应，登录失败,请输入Y返回首页:";
				}
				continue;
			}
            if (recv_buf[0] == 'Y')   //登录成功
			{
                cout<<"登录成功"<<endl;
                succ();
                char qt[1005]="exit";
                isend = sendto(sClient, qt, strlen(qt), 0, (struct sockaddr*)&ser, ser_length);
                printf("退出成功，请输入Y返回首页:\n");
                string ret;
                while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "未注册用户名，登录失败，请输入Y返回首页:";
				}
				continue;
			}
            else if (recv_buf[0] == '0')
			{
				cout << "未注册用户名，登录失败，请输入Y返回首页:";
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "未注册用户名，登录失败，请输入Y返回首页:";
				}
				continue;
			}
            else if (recv_buf[0] == '1')
			{
				cout << "密码错误，登录失败，请输入Y返回首页:" << endl;
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "密码错误，登录失败，请输入Y返回首页:";
				}
				continue;
			}

        }
        else if(choice == "2")
        {
        	system("cls");
			cout << "请设置用户名:";
			string username;
			getline(cin, username);
			cout << "请设置登录密码:";
			string password;
			getline(cin, password);
			string init_infortemp = "R" + username + "#" + password;
			char *init_infor = new char[init_infortemp.length() + 1];
			strcpy(init_infor, init_infortemp.c_str());
			//向服务器发送注册用户信息
			isend = sendto(sClient, init_infor, init_infortemp.length() + 1, 0, (struct sockaddr*)&ser, ser_length);
			//接收服务器回应的消息
			irecv = recvfrom(sClient, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&com, &com_len);
            if (recv_buf[0] == 'Y')
			{
				cout << "注册成功" << endl;
				continue;
			}
			else
			{
				cout << "用户名已存在，注册失败，请输入Y返回首页:" << endl;
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "用户名已存在，注册失败，请输入Y返回首页:";
				}
				continue;
			}
        }
        else if (choice == "3")
		{
			closesocket(sClient);
			WSACleanup;
			return;
		}
        else continue;
    }
}
int main()
{
    client x;
    if(x.Startup()== false)
    {
        printf("Startup faild");
        return 0;
    }
    x.SetServerAddress();
    x.work();
    system("pause");
    return 0;
}
