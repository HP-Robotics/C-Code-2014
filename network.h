#ifndef _NETWORK_H
#define _NETWORK_H

#include "networkexception.h"
#include "networkmessage.h"

#include <cstddef>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <vector>

using namespace std;

#define BUFLEN 1500
#define MSGLEN 1498

class networkrobot
{

protected:
	bool _connected;

	static unsigned char _buf[BUFLEN];
	
	int _outsocket;
	int _insocket;
	struct sockaddr_in _outaddr;
	struct sockaddr_in _inaddr;

	const char* _robotaddr;
	const unsigned short _robotport;
	const unsigned short _listenport;
	
	void int_send(const unsigned char* message, int len);
	
public:
	networkrobot(const char* robotaddr, unsigned short robotport, unsigned short listenport);
	
	void connect();
	
	void sendmessage(networkmessage const*);
	
	networkmessage* receivemessage(bool block);
	
	
	
	/*template<class T>
	inline void send(const T& networkmessage)
	{
		int_send(&message, 1);
	}*/
	

	/*template<class T>
	bool receive(T* ret)
	{
		if(!_connected)
			throw notconnectedexception();

		int bytes = recv(_insocket, _buf, BUFLEN, MSG_DONTWAIT);
	
		if(bytes == -1)
			return false;
		
		int messagelength = _buf[0];
		for(int i = 0; i < messagelength; i++)
		{
			*(((void*)&ret)+i) = _buf[i+1];
		}
		
		return true;
	}*/
	
};

/*template<>
	inline void networkrobot::send<string>(const string& message)
	{
		int_send<char>(message.c_str(), (int)message.length());
	}*/





#endif
