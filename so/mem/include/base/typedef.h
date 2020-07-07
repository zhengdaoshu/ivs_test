/*
 * typedef.h
 *
 *  Created on: 2016年11月24日
 *      Author: J00054
 */

#ifndef INCLUDE_BASE_TYPEDEF_H_
#define INCLUDE_BASE_TYPEDEF_H_

//基础类型定义
typedef unsigned char		u8;
typedef char				s8;
typedef unsigned short		u16;
typedef short				s16;
typedef unsigned int		u32;
typedef int					s32;
typedef float				FP32;
typedef double				FP64;
typedef void*				PVOID;
typedef unsigned long long  u64;
typedef long long			s64;
typedef int                 BOOL;

/* 内存规格 */
#define KB (1024)
#define MB (1024 * 1024)

#define IN
#define OUT
#define INOUT


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#endif /* INCLUDE_BASE_TYPEDEF_H_ */
