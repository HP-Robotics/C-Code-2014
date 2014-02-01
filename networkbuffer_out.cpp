#include "networkbuffer_out.h"

#include <cstring>
#include <cstdlib>
#include <string>
#include <arpa/inet.h>

networkbuffer_out::networkbuffer_out(const unsigned char* raw, int length)
	:_buffer(0), _size(length), _offset(0)
{
	_buffer = (unsigned char*)malloc((length+1) * sizeof(char));
	memcpy(_buffer, raw, length);
	_buffer[length] = 0;
}


int networkbuffer_out::getSize() const
{
	return _size;
}

networkbuffer_out& networkbuffer_out::operator>>(bool& v)
{
	return operator>>((unsigned char&)v);
}
networkbuffer_out& networkbuffer_out::operator>>(char& v )
{
	return operator>>((unsigned char&)v);
}
networkbuffer_out& networkbuffer_out::operator>>(short& v)
{
	return operator>>((unsigned short&)v);
}
networkbuffer_out& networkbuffer_out::operator>>(int& v)
{
	return operator>>((unsigned int&)v);
}
networkbuffer_out& networkbuffer_out::operator>>(unsigned char& v)
{
	v = _buffer[_offset++];
	return *this;
}
networkbuffer_out& networkbuffer_out::operator>>(unsigned short& v)
{
	unsigned char big, small;
	big = _buffer[_offset++];
	small = _buffer[_offset++];
	unsigned short network = (((short)big) << 8) | small;
	
	v = ntohs(network);
	return *this;
}
networkbuffer_out& networkbuffer_out::operator>>(unsigned int& v)
{
	//sorry about these variable names :D
	unsigned char bigest, big, small, smallest;
	bigest = _buffer[_offset++];
	big = _buffer[_offset++];
	small = _buffer[_offset++];
	smallest = _buffer[_offset++];
	unsigned int network = (((int)bigest) << 24) | (((int)big) << 16) | (((int)small) << 8) | smallest;
	
	v = ntohs(network);
	return *this;
}
networkbuffer_out& networkbuffer_out::operator>>(string& v)
{
	v = string((const char*)_buffer + _offset);
	_offset += strlen((const char*)_buffer + _offset);
	return *this;
}
int networkbuffer_out::copyfrom(char* buffer, int bytestocopy)
{
	int remaining = _size - _offset;
	if(remaining >= bytestocopy)
	{
		memcpy(buffer, (_buffer + _offset), bytestocopy);
		_offset += bytestocopy;
		return bytestocopy;
	}
	else
	{
		memcpy(buffer, (_buffer + _offset), remaining);
		_offset += remaining;
		return remaining;
	}
	
}
