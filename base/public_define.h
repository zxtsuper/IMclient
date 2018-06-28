#ifndef __PUBLIC_DEFINE_H__
#define __PUBLIC_DEFINE_H__


// client type;
#define CLIENT_TYPE_FLAG_NONE	0x00
#define CLIENT_TYPE_FLAG_PC	0x01
#define CLIENT_TYPE_FLAG_MOBILE 0x02
#define CLIENT_TYPE_FLAG_BOTH	0x03

typedef struct {
	uint32_t user_id;
	uint32_t conn_cnt;
}user_conn_t;

typedef struct {
	uint32_t user_id;
	uint32_t status;
	uint32_t client_type;
}user_stat_t;

typedef struct {
	string id;
	string name;
}robot_stat_t;


#define MAX_MSG_LEN	4096

#endif
