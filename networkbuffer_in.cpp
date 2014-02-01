#include "networkbuffer_in.h"
#include "networkexception.h"

#include <arpa/inet.h>
#include <stdio.h>

networkbuffer_in::networkbuffer_in(int size)
	:_buffer(), _size(size)
{
}
int networkbuffer_in::getSize() const
{
	return _size;
}
const unsigned char* networkbuffer_in::getBuffer() const
{
	return &_buffer[0];
}
const int networkbuffer_in::getDataLength() const
{
	return _buffer.size();
}

void networkbuffer_in::safe_push_back(unsigned char c)
{
	if(getDataLength()==getSize())
		throw bufferexception(getSize(), getDataLength());
	_buffer.push_back(c);
}

networkbuffer_in& networkbuffer_in::operator<<(bool v)
{
	return operator<<((unsigned char)v);
}

networkbuffer_in& networkbuffer_in::operator<<(int v)
{
	return operator<<((unsigned int)v);
}
networkbuffer_in& networkbuffer_in::operator<<(short v)
{
	return operator<<((unsigned short)v);
}
networkbuffer_in& networkbuffer_in::operator<<(char v)
{
	return operator<<((unsigned char)v);
}
networkbuffer_in& networkbuffer_in::operator<<(unsigned int v)
{
	unsigned int conv = htonl(v);
	safe_push_back((conv >> 24) & 0xff);
	safe_push_back((conv >> 16) & 0xff);
	safe_push_back((conv >> 8) & 0xff);
	safe_push_back(conv & 0xff);
	return *this;
}
networkbuffer_in& networkbuffer_in::operator<<(unsigned short v)
{
	unsigned short conv = htons(v);
	safe_push_back((conv >> 8) & 0xff);
	safe_push_back(conv & 0xff);
	return *this;
}
networkbuffer_in& networkbuffer_in::operator<<(unsigned char v)
{
	safe_push_back(v);
	return *this;
}

networkbuffer_in& networkbuffer_in::operator<<(const string& v)
{
	return operator<<(v.c_str());
}
networkbuffer_in& networkbuffer_in::operator<<(const char* v)
{
	while(*v){
		safe_push_back((unsigned char)*v);
		++v;
	}
	
	return *this;
}

networkbuffer_in& networkbuffer_in::copyto(const unsigned char* v, int len)
{
	for(int i = 0; i < len; i++)
		safe_push_back(v[i]);
		
	return *this;
}


