#include "network.h"
#include "networkexception.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <errno.h>

#ifdef _WRS_KERNEL

#include <inetLib.h>
#include <selectLib.h>
#include <sockLib.h>
#include <taskLib.h>
#include <usrLib.h>
#include <ioLib.h>

#endif

unsigned char networkrobot::_buf[BUFLEN];


networkrobot::networkrobot(const char* robotaddr, unsigned short robotport, unsigned short listenport)
	:_connected(false), _outsocket(-1), _insocket(-1), _robotaddr(robotaddr), _robotport(robotport), _listenport(listenport)
{
	connect();
}


void networkrobot::connect()
{
	_outsocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(_outsocket == -1)
		throw socketexception();
	
	_outaddr.sin_family = AF_INET;
	//const cast is WPILib hack (compatible)
    _outaddr.sin_addr.s_addr = inet_addr(const_cast<char*>(_robotaddr));
    _outaddr.sin_port  = htons(_robotport);
    
    
    _insocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(_insocket == -1)
		throw socketexception();
		
	memset((void*)&_inaddr, 0, sizeof(_inaddr));
    
    _inaddr.sin_family = AF_INET;
	_inaddr.sin_port = htons(_listenport);
	_inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(_insocket, (struct sockaddr*)&_inaddr, sizeof(_inaddr)) == -1)
		throw bindingexception();
		
	_connected = true;
	
}

void networkrobot::int_send(const unsigned char* message, int len)
{
	if(!_connected)
		throw notconnectedexception();

	if(len > MSGLEN)
		throw bufferexception(MSGLEN, len);
	unsigned short messagelength = (unsigned short)len;
	//load the messagelength then the data to the buffer
	unsigned short netmessagelen = htons(messagelength);
	_buf[0] = (netmessagelen >> 8) & 0xff;
	_buf[1] = netmessagelen & 0xff;
	for(int i = 0; i < messagelength; i++)
		_buf[i+2] = message[i];
#ifdef _WRS_KERNEL
	int rc = sendto(_outsocket, (char*)_buf, messagelength+2, 0, (struct sockaddr *) &_outaddr, sizeof(_outaddr));
#else
	int rc = sendto(_outsocket, (const void*)_buf, messagelength+2, 0, (struct sockaddr *) &_outaddr, sizeof(_outaddr));
#endif
	if(rc == -1)
		throw sendexception();
}

void networkrobot::sendmessage(networkmessage const* msg)
{
	networkbuffer_in messagedata(MSGLEN-2);
	msg->serialize(messagedata);
	if(messagedata.getSize() > MSGLEN-2)
		throw messagelengthexceededexception();
	GUID netid = ENCODEGUID(msg->getGuid());
	vector<unsigned char> data;
	//write the guid
	data.push_back((netid >> 8) & 0xff);
	data.push_back(netid & 0xff);
	//write the data from the buffer
	data.insert(data.end(), &messagedata.getBuffer()[0], &messagedata.getBuffer()[messagedata.getDataLength()]);
	
	int_send(&data[0], data.size());
}


networkmessage* networkrobot::receivemessage(bool block)
{
	if(!_connected)
		throw notconnectedexception();
#ifdef _WRS_KERNEL
	int bytes = recv(_insocket, (char*)_buf, BUFLEN, block ? 0 : MSG_DONTWAIT);
#else
	int bytes = recv(_insocket, _buf, BUFLEN, block ? 0 : MSG_DONTWAIT);
#endif

	if(bytes == -1)
		return 0;
	
	int messagelength = ntohs((short)(((unsigned char)_buf[0]) << 8 | ((unsigned char)_buf[1])));
	int guid = DECODEGUID((short)(((unsigned char)_buf[2]) << 8 | ((unsigned char)_buf[3])));
	
	return networkmessage::createmessage(guid, _buf+4, messagelength-2);
}
	
	

	


