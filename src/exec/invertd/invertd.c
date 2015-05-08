/******************************************************************************
 ** Coypright(C) 2014-2024 Xundao technology Co., Ltd
 **
 ** 文件名: invert.c
 ** 版本号: 1.0
 ** 描  述: 
 ** 作  者: # Qifeng.zou # Wed 06 May 2015 10:22:56 PM CST #
 ******************************************************************************/

#include "invertd.h"
#include "invtd_priv.h"
#include "invtd_conf.h"

/******************************************************************************
 **函数名称: main 
 **功    能: 倒排服务主程序
 **输入参数:
 **     argc: 参数个数
 **     argv: 参数列表
 **输出参数: NONE
 **返    回: 0:成功 !0:失败
 **实现描述:
 **注意事项: 
 **作    者: # Qifeng.zou # 2015.05.07 #
 ******************************************************************************/
int main(int argc, char *argv[])
{
    invtd_opt_t opt;
    invtd_cntx_t *ctx;

    /* > 获取参数 */
    if (invtd_getopt(argc, argv, &opt))
    {
        return invtd_usage(basename(argv[0])); /* 显示帮助 */
    }

    if (opt.isdaemon)
    {
        /* int daemon(int nochdir, int noclose);
         *  1. daemon()函数主要用于希望脱离控制台, 以守护进程形式在后台运行的程序.
         *  2. 当nochdir为0时, daemon将更改进城的根目录为root(“/”).
         *  3. 当noclose为0时, daemon将进城的STDIN, STDOUT, STDERR都重定向到/dev/null */
        daemon(1, 1);
    }

    /* > 服务初始化 */
    ctx = invtd_init(opt.conf_path);
    if (NULL == ctx)
    {
        fprintf(stderr, "Init invertd failed!\n");
        return -1;
    }

    /* > 启动服务 */
    if (invtd_startup(ctx))
    {
        log_fatal(ctx->log, "Startup invertd failed!");
        return -1;
    }

    while (1) { pause(); }

    return 0;
}