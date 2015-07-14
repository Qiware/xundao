#if !defined(__CONF_H__)
#define __CONF_H__

#include "comm.h"
#include "list.h"

#define SYS_CONF_DEF_PATH "../conf/sys_conf.xml"

/* 映射配置 */
typedef struct
{
    char name[NODE_MAX_LEN];            /* 结点名 */
    char path[FILE_PATH_MAX_LEN];       /* 结点对应的配置路径 */
} conf_map_t;

/* 系统配置 */
typedef struct
{
    list_t *listen;                     /* 侦听配置 */
    list_t *frwder;                     /* 转发配置 */
} sys_conf_t;

int conf_load_system(const char *fpath, sys_conf_t *conf);

#endif /*__CONF_H__*/
