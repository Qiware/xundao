#if !defined(__PROB_MESG_H__)
#define __PROB_MESG_H__

#include <stdint.h>

/* 报头 */
typedef struct
{
    unsigned int type;                      /* 消息类型 Range: mesg_type_e */
#define PROB_MSG_FLAG_SYS   (0)             /* 0: 系统数据类型 */
#define PROB_MSG_FLAG_USR   (1)             /* 1: 自定义数据类型 */
    unsigned int flag;                      /* 标识量(0:系统数据类型 1:自定义数据类型) */
    unsigned int length;                    /* 报体长度 */
#define PROB_MSG_MARK_KEY   (0x1ED23CB4)
    unsigned int mark;                      /* 校验值 */
} prob_mesg_header_t;

#endif /*__PROB_MESG_H__*/