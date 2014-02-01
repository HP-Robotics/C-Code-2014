#define REGISTERCLASS(guid, classname) if(id == guid) {return new classname();}

typedef unsigned short GUID;
#define ENCODEGUID(id) htons(id)
#define DECODEGUID(id) ntohs(id)
