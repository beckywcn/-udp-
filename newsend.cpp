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
	int GeneratePort();               //���������ʾ���˿ں�
	bool Getonlinelist();             //������ߵ��û���
	void work();                      //��������������
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
	cout << "������ip��ַ:";
	string iptemp;
	cin >> iptemp;
	char *ip = new char[iptemp.length() + 1];
	strcpy(ip, iptemp.c_str());
	//�����������˵�ַ
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
		cout << "*       1.��¼  2.ע��  3.�˳�         *" << endl;
		cout << "*                                      *" << endl;
		cout << "****************************************" << endl;
        string choice;
        getline(cin,choice);
        if(choice=="1")
        {
            system("cls");
            cout << "�������û���:";
			string username;
			getline(cin, username);
			cout << "����������:";
			string password;
			getline(cin, password);
            receiver_port = GeneratePort();
            ofstream out("port.txt");
			out << receiver_port << "\n" << username;
			out.close();
            string init_infortemp = "L" + username + "#" + password + "#" + tostring(receiver_port);
            char *init_infor = new char[init_infortemp.length() + 1];
			strcpy(init_infor, init_infortemp.c_str());
			//���������֤�û���Ϣ
			isend = sendto(sClient, init_infor, init_infortemp.length() + 1, 0, (struct sockaddr*)&ser, ser_length);
			//���շ�������Ӧ����Ϣ
			irecv = recvfrom(sClient, recv_buf, sizeof(recv_buf), 0, (SOCKADDR*)&com, &com_len);
            if (irecv == SOCKET_ERROR)
			{
				cout << "recvfrom() Failed:" << GetLastError() << endl;
				cout << "δ�յ�����������Ӧ����¼ʧ��,������Y������ҳ:";
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "δ�յ�����������Ӧ����¼ʧ��,������Y������ҳ:";
				}
				continue;
			}
            if (recv_buf[0] == 'Y')   //��¼�ɹ�
			{
                cout<<"��¼�ɹ�"<<endl;
                succ();
                char qt[1005]="exit";
                isend = sendto(sClient, qt, strlen(qt), 0, (struct sockaddr*)&ser, ser_length);
                printf("�˳��ɹ���������Y������ҳ:\n");
                string ret;
                while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "δע���û�������¼ʧ�ܣ�������Y������ҳ:";
				}
				continue;
			}
            else if (recv_buf[0] == '0')
			{
				cout << "δע���û�������¼ʧ�ܣ�������Y������ҳ:";
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "δע���û�������¼ʧ�ܣ�������Y������ҳ:";
				}
				continue;
			}
            else if (recv_buf[0] == '1')
			{
				cout << "������󣬵�¼ʧ�ܣ�������Y������ҳ:" << endl;
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "������󣬵�¼ʧ�ܣ�������Y������ҳ:";
				}
				continue;
			}

        }
        else if(choice == "2")
        {
        	system("cls");
			cout << "�������û���:";
			string username;
			getline(cin, username);
			cout << "�����õ�¼����:";
			string password;
			getline(cin, password);
			string init_infortemp = "R" + username + "#" + password;
			char *init_infor = new char[init_infortemp.length() + 1];
			strcpy(init_infor, init_infortemp.c_str());
			//�����������ע���û���Ϣ
			isend = sendto(sClient, init_infor, init_infortemp.length() + 1, 0, (struct sockaddr*)&ser, ser_length);
			//���շ�������Ӧ����Ϣ
			irecv = recvfrom(sClient, recv_buf, sizeof(recv_buf), 0, (struct sockaddr*)&com, &com_len);
            if (recv_buf[0] == 'Y')
			{
				cout << "ע��ɹ�" << endl;
				continue;
			}
			else
			{
				cout << "�û����Ѵ��ڣ�ע��ʧ�ܣ�������Y������ҳ:" << endl;
				string ret;
				while (getline(cin, ret))
				{
					if (ret == "Y")break;
					cout << "�û����Ѵ��ڣ�ע��ʧ�ܣ�������Y������ҳ:";
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
