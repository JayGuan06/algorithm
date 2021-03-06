#include "protocol.h"



class APPProtocollIMPL
{
public:
	APPProtocollIMPL();

	void IntToByteHex(char* byte, const int number);//大端 网络字节序

	int ByteHexToInt(const char* arry, const int postion);//big 大端 网络字节序

	LXS_MSG &GetMsg() { return msg; }

	void AddToMsg(char * msg_pt, char *byte, int begin, int end);

	void ProtocolParser(char * data);

	void SetMsgSize(int size);//msg num

	bool CheckSize();

	void InsertMsg(int msg_id, int msg_length);
	void InsertMsg(int msg_id, int msg_length, char * body);
	void InsertMsg(int msg_id, int msg_length, int body);

	void SetHead(char* msg_ptr, int msg_id, int msg_length);

	~APPProtocollIMPL();
private:
	LXS_MSG msg;
	int insert_postion;
	int msg_num;
};




APPProtocol::APPProtocol() :_impl(new APPProtocollIMPL)
{	
}

LXS_MSG & APPProtocol::GetMsg()
{
	return this->_impl->GetMsg();
}

void APPProtocol::SetMsgSize(int size)
{
	this->_impl->SetMsgSize(size);
	
}


void APPProtocol::InsertMsg(int msg_id, int msg_length)
{
	this->_impl->InsertMsg(msg_id,msg_length);
}

void APPProtocol::InsertMsg(int msg_id,int msg_length,char * body)
{
	this->_impl->InsertMsg(msg_id,msg_length,body);
}

void APPProtocol::InsertMsg(int msg_id, int msg_length, int body)
{
	this->_impl->InsertMsg(msg_id, msg_length, body);
}


void APPProtocol::AddToMsg(char * msg_ptr, char *byte, int begin, int end)
{
	this->_impl->AddToMsg(msg_ptr,byte,begin,end);
}



void APPProtocol::ProtocolParser(char * msg)
{
	this->_impl->ProtocolParser(msg);
}

APPProtocol::~APPProtocol()
{
	this->_impl->~APPProtocollIMPL();
}



APPProtocollIMPL::APPProtocollIMPL() :insert_postion(0),msg_num(0)
{
	SetMsgSize(5);

	InsertMsg(1001, 8);
	InsertMsg(1002, 8);
	InsertMsg(1003, 808, const_cast<char*>("ABCDEFG"));
	InsertMsg(1004, 12, 1);
	InsertMsg(1005, 12, 1);

	//this->ProtocolParser(*(this->msg.msg_arr + 0));
	this->ProtocolParser(*(this->msg.msg_arr + 2));
	this->ProtocolParser(*(this->msg.msg_arr + 3));

}


void APPProtocollIMPL::SetMsgSize(int size)
{
	this->msg.msg_arr = new char*[size];
	this->msg_num = size;
}

bool APPProtocollIMPL::CheckSize()
{
	cout << "check"<< insert_postion << endl;
	cout << "msg_num" << this->msg_num << endl;
	if (insert_postion > this->msg_num - 1)
		return false;
	else
		return true;
	//TODO log
}


void APPProtocollIMPL::SetHead(char* msg_ptr, int msg_id, int msg_length)
{
	int begin = 0;
	int end = 0;
	char ch[4];
	IntToByteHex(ch, 8);
	AddToMsg(msg_ptr, ch, begin, end = begin + 4);
	begin = end;
	IntToByteHex(ch, msg_id);//
	AddToMsg(msg_ptr, ch, begin, end = begin + 4);
}

void APPProtocollIMPL::InsertMsg(int msg_id, int msg_length)
{
	if (CheckSize())
	{
		char ** msg_ptr = this->msg.msg_arr + this->insert_postion;
		*msg_ptr = new char[msg_length];
		memset(*msg_ptr, 0, msg_length);
		this->insert_postion += 1;

		SetHead(*msg_ptr, msg_id, msg_length);
	}
	else
	{
		//TODO log
		cout << "消息容器已满，无法继续添加！";
	}
}

void APPProtocollIMPL::InsertMsg(int msg_id, int msg_length, char * body)
{
	if (CheckSize())
	{
		char ** msg_ptr = this->msg.msg_arr + this->insert_postion;
		*msg_ptr = new char[msg_length];
		memset(*msg_ptr, 0, msg_length);
		this->insert_postion += 1;

		SetHead(*msg_ptr, msg_id, msg_length);

		if (body != NULL)
			AddToMsg(*msg_ptr, body, 8, 8 + strlen(body));
	}
	else
	{
		//TODO log
		cout << "消息容器已满，无法继续添加！";
	}
}

void APPProtocollIMPL::InsertMsg(int msg_id, int msg_length, int body)
{
	if (CheckSize())
	{
		char ** msg_ptr = this->msg.msg_arr + this->insert_postion;
		*msg_ptr = new char[msg_length];
		memset(*msg_ptr, 0, msg_length);
		this->insert_postion += 1;

		SetHead(*msg_ptr, msg_id, msg_length);

		if (body != -1)
		{
			char ch[4];
			IntToByteHex(ch, body);//
			AddToMsg(*msg_ptr, ch, 8, 8 + 4);
		}
	}
	else
	{
		//TODO log
		cout << "消息容器已满，无法继续添加！";
	}

}


void APPProtocollIMPL::AddToMsg(char * msg_ptr, char *byte, int begin, int end)
{
	if (msg_ptr != NULL)
	{
		for (int i = begin; i < end; i++)
		{
			msg_ptr[i] = byte[i - begin];
		}

	}
}



void APPProtocollIMPL::IntToByteHex(char* byte, const int number)//大端 网络字节序
{
	memset(byte, 0, 4);
	byte[3] = (unsigned char)(0x000000ff & number);
	byte[2] = (unsigned char)((0x0000ff00 & number) >> 8);
	byte[1] = (unsigned char)((0x00ff0000 & number) >> 16);
	byte[0] = (unsigned char)((0xff000000 & number) >> 24);

}

//@postion 起始位置
int APPProtocollIMPL::ByteHexToInt(const char * arry, const int postion)//big 大端 网络字节序
{
	int value;
	value = (int)((arry[postion + 3] & 0xFF)
		| ((arry[postion + 2] << 8) & 0xFF00)
		| ((arry[postion + 1] << 16) & 0xFF0000)
		| ((arry[postion + 0] << 24) & 0xFF000000));
	return value;
}

void APPProtocollIMPL::ProtocolParser(char * msg)
{
	cout << "address:" << msg << endl;
	int ack = 0;
	int msg_id = ByteHexToInt(msg, 4);
	std::cout << "msg_id:" << msg_id;

	switch (msg_id)
	{
	case 1001:
		break;
	case 1002:
		break;
	case 1003:
		char data[800];
		memset(data, 0, 800);
		for (int i = 0; i < 800; i++)
			data[i] = msg[i + 8];
		cout << "MSG:" << data;
		break;
	case 1004:
		ack = ByteHexToInt(msg, 8);
		cout << "ack:" << ack;
		break;
	case 1005:
		ack = ByteHexToInt(msg, 8);
		break;

	}


}

APPProtocollIMPL::~APPProtocollIMPL()
{
}
