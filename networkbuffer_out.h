#ifndef _NETWORKBUFFER_OUT
#define _NETWORKBUFFER_OUT

#include <string>

using std::string;

class networkbuffer_out
{

protected:
	unsigned char* _buffer;
	int _size;
	int _offset;
	
public:
	networkbuffer_out(const unsigned char* raw, int size);
	int getSize() const;
	
	networkbuffer_out& operator>>(bool&);
	networkbuffer_out& operator>>(char&);
	networkbuffer_out& operator>>(short&);
	networkbuffer_out& operator>>(int&);
	networkbuffer_out& operator>>(unsigned char&);
	networkbuffer_out& operator>>(unsigned short&);
	networkbuffer_out& operator>>(unsigned int&);
	networkbuffer_out& operator>>(string&);
	int copyfrom(char* buffer, int bytestocopy);
};


#endif
