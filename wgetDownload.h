#ifndef __WGETDOWNLOAD_H
#define __WGETDOWNLOAD_H
typedef enum{
    VOLT = 1,
    CONF = 2
}FILETYPE;

typedef enum{
    SILENT = 3,
    NO_SILENT = 4
}QuietMode;

typedef enum{
    CONTINUE = 5,
    NO_CONTINUE =6
}ConMode;

char* wget_download(char* filepath, QuietMode qmode, ConMode cmode, char* type);

#endif
