#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include <stdlib.h>

#include "shell.h"
#include "mem_mgnt.h"

/*Hello world!*/
int cmd_helloworld(int argc, char *argv[])
{
    shellPrint(shellGetCurrent(), "%s\n", "helloworld");
    shellWriteString(shellGetCurrent(), "helloworld\r\n");
    shellPrint(shellGetCurrent(), "we have %d parameters\n", argc);
    for (int i = 0; i < argc; i++)
    {
        shellPrint(shellGetCurrent(), "argv[%d]:%s\n", i, argv[i]);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), helloworld, cmd_helloworld, helloworld);

/*ADD*/
int cmd_add(int argc, char *argv[])
{
    if (argc != 3)
    {
        shellWriteString(shellGetCurrent(), "parameter error!\n");
        return -1;
    }

    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    int sum = a + b;

    shellPrint(shellGetCurrent(), "%s\n", "ADD");
    shellPrint(shellGetCurrent(), "%d + %d = %d\r\n", a, b, sum);

    shellPrint(shellGetCurrent(), "we have %d parameters\n", argc);
    for (int i = 0; i < argc; i++)
    {
        shellPrint(shellGetCurrent(), "argv[%d]:%s\n", i, argv[i]);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), add, cmd_add, add);

/*mem*/
int cmd_memapply(int argc, char *argv[])
{
    int size = atoi(argv[1]);
    uint8_t *ptr =malloc(size);
    shellPrint(shellGetCurrent(), "allocated %d bytes at 0x%p\n", size, ptr);
    free(ptr);

    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), memapply, cmd_memapply, applyformem);