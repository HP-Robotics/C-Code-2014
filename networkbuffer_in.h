#ifndef _NETWORKBUFFER_IN_H
#define _NETWORKBUFFER_IN_H

#include <vector>
#include <string>

using std::string;
using std::vector;

class networkbuffer_in
{
protected:
	vector<unsigned char> _buffer;
	int _size;
	void safe_push_back(unsigned char);
public:
	networkbuffer_in(int size);
	int getSize() const;
	const unsigned char* getBuffer() const;
	const int getDataLength() const;
	networkbuffer_in& operator<<(bool);
	networkbuffer_in& operator<<(int);
	networkbuffer_in& operator<<(short);
	networkbuffer_in& operator<<(char);
	networkbuffer_in& operator<<(unsigned int);
	networkbuffer_in& operator<<(unsigned short);
	networkbuffer_in& operator<<(unsigned char);
	networkbuffer_in& operator<<(const string&);
	networkbuffer_in& operator<<(const char*);
	networkbuffer_in& copyto(const unsigned char*, int len);
	
};

#endif
