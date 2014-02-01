#ifndef _NETWORKEXCEPTION_H
#define _NETWORKEXCEPTION_H

#include "guid.h"

#include <exception>
#include <string>
#include <sstream>

using namespace std;

class bufferexception : public exception
{
public:
	bufferexception(int max, int actual)
	{
		maxsize = max;
		actualsize = actual;
	}

	int maxsize;
	int actualsize;
	virtual const char* what() const throw()
	{
		std::ostringstream s;
		s << "Buffer size exceeded, max size: " << maxsize << ", actual size: " << actualsize << "\n";
		return s.str().c_str();
	}
};

class socketexception : public exception
{
	virtual const char* what() const throw()
	{
		return "Socket creation failed";
	}
};

class sendexception : public exception
{
	virtual const char* what() const throw()
	{
		return "Sendto failed";
	}
};

class bindingexception : public exception
{
	virtual const char* what() const throw()
	{
		return "Binding failed";
	}
};

class notconnectedexception : public exception
{
	virtual const char* what() const throw()
	{
		return "Not connected";
	}
};

class messagelengthexceededexception : public exception
{
	virtual const char* what() const throw()
	{
		return "Message cannot be longer than 253 bytes";
	}
};

class guidunregisteredexception : public exception
{
public:
	guidunregisteredexception(GUID id)
	{
		_id = id;
	}
	GUID _id;
	virtual const char* what() const throw()
	{
		std::ostringstream s;
		s << "Unregistered GUID (" << _id << ")";
		return s.str().c_str();
	}
};

class notloadedexception : public exception
{
	virtual const char* what() const throw()
	{
		return "Message not loaded. Call deserialize first";
	}
};

class deserializationexception : public exception
{
	virtual const char* what() const throw()
	{
		return "Serialization error, data invalid";
	}
};


#endif
