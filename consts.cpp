#include "consts.h"
#include<string.h>
EXPORT bool LibVer(char*ret, int size)
{
    if(size > 5)
    {
        memset(ret,0,size);
        memcpy(ret,MTCLIB_VERSION,5);
        return true;
    }else
    {
        return false;
    }
}
