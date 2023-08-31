#ifndef __UTIL_H__
#define __UTIL_H__

//为节省代码空间，
//常数字节交换建议使用反转宏
//变量字节交换建议使用反转函数

#define REV4(d)     ((((DWORD)(d) & 0x000000ff) << 24) | \
                     (((DWORD)(d) & 0x0000ff00) << 8)  | \
                     (((DWORD)(d) & 0x00ff0000) >> 8)  | \
                     (((DWORD)(d) & 0xff000000) >> 24))

#define REV2(d)     ((((WORD)(d) & 0x00ff) << 8) | \
                     (((WORD)(d) & 0xff00) >> 8))
                    
DWORD reverse4(DWORD d);
WORD reverse2(WORD w);

#endif
