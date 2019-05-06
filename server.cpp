#include<WinSock2.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<cstdlib>
using namespace std;

#define DEFAULT_PORT 5055
#define BUFFER_LENGTH 1024

class user
{
public:
	user(string username, string ip, int sender_port, int receiver_port)
	{
		this->username = username;
		this->ip = ip;
		this->sender_port = sender_port;
		this->receiver_port = receiver_port;

		//设置接收器的地址
		receiver.sin_family = AF_INET;
		receiver.sin_port = htons(receiver_port);
		char *addr = new char[ip.length() + 1];
		strcpy(addr, ip.c_str());
		receiver.sin_addr.s_addr = inet_addr(addr);
	}
	string username;              //用户名
	string ip;                    //客户端ip地址
	int sender_port;              //发送器端口
	int receiver_port;	          //接收器端口
	struct sockaddr_in receiver;  //存储接收器的地址
};

class server
{
public:
    bool Startup();
    bool SetServerSocket();
    bool Checktxt();
    void work();
    void SendMessage(string message,struct sockaddr_in x);
    bool TestUsernameAndPassword(string username, string password, int &flag); 
    bool TestDuplicateRigister(string username);                      //测试是否重复注册
    int  Getuserindex(string username); 
    string Getusername(string ip, int port);   
    void extractLoginuserinfor(string userinfor, string &username, string &password, string &receiverport); //提取登录请求中的用户名密码和显示器端口号
	void extractRegisteruserinfor(string userinfor, string&username, string&password); 
private:
    WSADATA wsaData;
    SOCKET sSocket;
    struct sockaddr_in ser,cli;
    int cli_length = sizeof(cli);
    char recv_buf[BUFFER_LENGTH];
    string sendmessage,printmessage;
    int iSend,iRecv;
    vector<user> usertable;

};
bool server::Startup()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "Failed to load Winsock." << endl;
		return false;
	}
	return true;
}
bool server::SetServerSocket()
{
	//产生服务器端套接口
	sSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (sSocket == INVALID_SOCKET)
	{
		cout << "socket()Failed:" << WSAGetLastError() << endl;
		return false;
	}
	//建立服务器端地址
	ser.sin_family = AF_INET;
	ser.sin_port = htons(DEFAULT_PORT);               //htons()函数把一个双字节主机字节顺序的数转换为网络字节顺序的数
	ser.sin_addr.s_addr = htonl(INADDR_ANY);          //htonl()函数把一个主机字节顺序的数转换为网络字节顺序的数   
	if (bind(sSocket, (LPSOCKADDR)&ser, sizeof(ser)) == SOCKET_ERROR)
	{
		cout << "bind()Failed:" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
string server::Getusername(string ip, int port)
{
	for (int i = 0; i < usertable.size(); i++)
		if (usertable[i].ip == ip&&usertable[i].sender_port == port)
			return usertable[i].username;
	cout << "非法的用户连接上服务器" << endl;
	cout << "ip地址为:" << ip << endl << "端口号为:" << port << endl;
	return "";
}
int server::Getuserindex(string username)
{
	int i = 0;
	for (i = 0; i < usertable.size(); i++)
		if (usertable[i].username == username) break;
	return i;
}
void server::SendMessage(string message, struct sockaddr_in x)
{
	char *send_buf = new char[message.length() + 1];
	strcpy(send_buf, message.c_str());
	SOCKET rSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (rSocket == INVALID_SOCKET)
	{
		cout << "socket()Failed:" << WSAGetLastError() << endl;
		return;
	}
	iSend = sendto(rSocket, send_buf, message.length() + 1, 0, (SOCKADDR*)&(x), sizeof(x));
	if (iSend == SOCKET_ERROR)
	{
		cout << "sendto failed:" << WSAGetLastError() << endl;
		closesocket(rSocket);
		return;
	}
	closesocket(rSocket);
}
void server::extractLoginuserinfor(string userinfor, string &username, string &password, string &receiverport)
{
	int i;
	for (i = 0; i < userinfor.length(); i++)           //提取用户名
	{
		if (userinfor[i] == '#')
		{
			username = userinfor.substr(0, i);
			break;
		}
	}
	for (int j = i + 1; j < userinfor.length(); j++)  //提取密码和显示器端口号
	{
		if (userinfor[j] == '#')
		{
			password = userinfor.substr(i + 1, j - i - 1);
			receiverport = userinfor.substr(j + 1);
			break;
		}
	}
}
void server::extractRegisteruserinfor(string userinfor, string&username, string&password)
{
	for (int i = 0; i < userinfor.size(); i++)
	{
		if (userinfor[i] == '#')
		{
			username = userinfor.substr(0, i);
			password = userinfor.substr(i + 1);
			break;
		}
	}
}
bool server::TestUsernameAndPassword(string username, string password, int &flag)
{
	if (!Checktxt())
	{
		cout << "无法找到存储文件." << endl << endl;
		flag = 0;                                 //未找到用户名的标志
		return false;
	}
	fstream in("D:\\codedam\\info.txt");
	string line;
	string username_txt, password_txt;
	while (getline(in, line))
	{
		for (int i = 0; i < line.size(); i++)
		{
			if (line[i] == '#')
			{
				username_txt = line.substr(0, i);
				password_txt = line.substr(i + 1);
				break;
			}
		}
		if (username_txt == username)         //该用户名存在
		{
			if (password == password_txt)     //且密码正确
			{
				in.close();
				return true;                  //返回验证成功
			}
			cout << "用户" << username << "登录密码错误" << endl << endl;       //返回密码错误的信息
			flag = 1;                        //密码错误的标志
			return false;
		}
	}
	in.close();
	cout << "未注册过的用户:" << username << endl << endl;
	flag = 0;                                 //未找到用户名的标志
	return false;
}
bool server::TestDuplicateRigister(string username)
{
	if (!Checktxt())
	{
		cout << "无法找到存储文件." << endl << endl;
		return true;
	}
	fstream in("D:\\codedam\\info.txt");
	string line;
	while (getline(in, line))
	{
		string username_txt;
		for (int i = 0; i < line.size(); i++)
		{
			if (line[i] == '#')
			{
				username_txt = line.substr(0, i);              //提取用户名
				if (username_txt == username)                  //对比，相等则表明已经注册过
				{
					in.close();
					cout << "用户名" << username << "重复注册" << endl << endl;
					return true;
				}
				break;                                         //否则继续对比下一个用户名
			}
		}
	}
	in.close();
	return false;                                               //代码执行到这说明该用户名还没有注册过
}
bool server::Checktxt()
{
	FILE *fp = fopen("D:\\codedam\\info.txt", "r");
	if (fp == NULL)
	{
		system("md D:\\codedam");
		ofstream out("D:\\codedam\\info.txt");
		if (!out)
			return false;
		out.close();
		return true;
	}
	return true;
}
void server::work()
{
    cout << "-----------------" << endl;
	cout << "Server running" << endl;
	cout << "-----------------" << endl;
    while(true)
    {
        memset(recv_buf,0,sizeof(recv_buf));
        iRecv = recvfrom(sSocket, recv_buf, BUFFER_LENGTH, 0, (struct sockaddr*)&cli, &cli_length);
        if (iRecv == SOCKET_ERROR)
		{
			cout << "recvfrom()Failed:" << WSAGetLastError() << endl;
			continue;
		}
        char *x = inet_ntoa(cli.sin_addr); string address(x);         //获取客户端ip
		int userport = ntohs(cli.sin_port);  
        string infortype = string(recv_buf);    
        if (infortype[0] == 'L')                                      //登录请求
		{
            string userinfor = infortype.substr(1);  
            	string username, password, receiver_port;
			extractLoginuserinfor(userinfor, username, password, receiver_port);  //提取用户名和密码
            	int flag = 0;
			if (!TestUsernameAndPassword(username, password, flag))
			{
				if (flag == 0)
					SendMessage("0", cli);
				if (flag == 1)
					SendMessage("1", cli);
				continue;
			}
            int receiver_port_int = atoi(receiver_port.c_str());
			user newuser(username, address, userport, receiver_port_int);
			usertable.push_back(newuser);

			printmessage = "(上线消息)" + newuser.username + "已上线";               //设置要打印的消息
			sendmessage = printmessage;                                           //设置要转发的消息	
			SendMessage("Y", cli);       
			cout << printmessage << endl;
			cout << "用户ip:" << address << endl;
			cout << "用户端口:" << userport << endl;
			cout << "当前在线人数:" << usertable.size() << endl << endl;
        }
        else if(infortype[0]=='R') 
        {
            string userinfor = infortype.substr(1);                  //除去消息类型		
			string username, password;
			extractRegisteruserinfor(userinfor, username, password); //提取用户名和密码

																	 //检测用户名是否已经注册过
			if (TestDuplicateRigister(username))
			{
				SendMessage("N", cli);
				continue;
			}
			//向文件写入新注册的用户名和密码
			if (!Checktxt())
			{
				SendMessage("N", cli);
				continue;
			}
            fstream out("D:\\codedam\\info.txt", ios::app);
			out << userinfor << endl;
			out.close();
			//发送注册成功的回应
			SendMessage("Y", cli);
			cout << "注册成功" << endl << "新用户名为:" << username << endl << endl;
			continue;
        }
        else if (infortype == "#")
        {
            string sendername = Getusername(address, userport);
			if (sendername == "") continue;
			int i = Getuserindex(sendername);
			if (i >= usertable.size() || i < 0) continue;
			SendMessage("exit", usertable[i].receiver);                                 //向该用户显示器发送退出命令
			usertable.erase(usertable.begin() + i);
			printmessage = "(下线消息)" + sendername + "已下线";                         //设置要打印的消息
			sendmessage = printmessage;                          
        }
        else
		{
		 cout<<infortype<<endl;
        cout << printmessage << endl;
		cout << "用户ip:" << address << endl;
		cout << "用户端口:" << userport << endl;
		cout << "当前在线人数:" << usertable.size() << endl << endl;
		//向客户端发送消息
		
			for (int i = 0; i < usertable.size(); i++)
			SendMessage(infortype, usertable[i].receiver);
		}
    }
}
int main()
{
    server x;
	if (x.Startup() == false)
		return 0;
	if (x.SetServerSocket() == false)
		return 0;
	x.work();
    return 0;
}
