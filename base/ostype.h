#ifndef __OS_TYPE_H__
#define __OS_TYPE_H__

#ifdef _WIN32

#else
	#ifdef __APPLE__
	
	#else
		#include <sys/epoll.h>
	#endif
	#include <pthread.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <errno.h>
	#include <unistd.h>
	#include <stdint.h>
	#include <signal.h>
	#include <unistd.h>
	#include <fcntl.h>
	#define closesocket close
	#define ioctlsocket ioctl
#endif

#include <stdexcept>

#ifdef __GNUC__
	#include <ext/hash_map>
	using namespace __gnu_cxx;
	namespace __gnu_cxx {
		template<> struct hash<std::string> {
			size_t operator()(const std::string& x) const {
				return hash<const char*>()(x.c_str());
			}
		};
	}
#else
	#include <hash_map>
	using namespace stdext;
#endif

#ifdef _WIN32

#else
	typedef int SOCKET;
	typedef int BOOL;
#ifndef __APPLE__
	const int TRUE = 1;
	const int FALSE =  0;
#endif
	const int SOCKET_ERROR = -1;
	const int INVALID_SOCKET = -1;
#endif

typedef unsigned char 	uchar_t;
typedef int		net_handle_t;
typedef int		conn_handle_t;

enum {
	NETLIB_OK	=0,
	NETLIB_ERROR	=-1
};

#define NETLIB_INVALID_HANDLE -1

enum {
	NETLIB_MSG_CONNECT = 1,
	NETLIB_MSG_CONFIRM,
	NETLIB_MSG_READ,
	NETLIB_MSG_WRITE,
	NETLIB_MSG_CLOSE,
	NETLIB_MSG_TIMER,
	NETLIB_MSG_LOOP
};

const uint32_t INVALID_UINT32 = (uint32_t)-1;
const uint32_t INVALID_VALUE = 0;

typedef void (*callback_t)(void* callback_data,uint8_t msg,uint32_t handle,void* param);

#endif
