#if !defined(__SRCH_MESG_H__)
#define __SRCH_MESG_H__

#include <stdint.h>

/* 报头 */
typedef struct
{
    unsigned int type:8;                    /* 消息类型 Range: mesg_type_e */
#define SRCH_MSG_FLAG_SYS   (0)             /* 0: 系统数据类型 */
#define SRCH_MSG_FLAG_USR   (1)             /* 1: 自定义数据类型 */
    unsigned int flag:8;                    /* 标识量(0:系统数据类型 1:自定义数据类型) */
    unsigned short length;                  /* 报体长度 */
#define SRCH_MSG_MARK_KEY   (0x1ED23CB4)
    unsigned int mark;                      /* 校验值 */
} srch_mesg_header_t;

#endif /*__SRCH_MESG_H__*/
