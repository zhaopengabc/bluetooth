/*  BEGIN_FILE_HDR
************************************************************************************************
*   NOTICE                              
*   
************************************************************************************************
*   File Name       : HRSTDDEF.h
************************************************************************************************
*   Project/Product : 
*   Title           : 
*   Author          : 
************************************************************************************************
*   Description     : 
*
************************************************************************************************
*   Limitations     : NONE
*
************************************************************************************************
*
************************************************************************************************
*   Revision History:
* 
*   Version       Date         Initials     CR#           Descriptions
*   ---------   -----------  ------------  ----------  ---------------
************************************************************************************************
*   END_FILE_HDR*/

#ifndef HR_STDDEF_H
#define HR_STDDEF_H

 /* data type defintion */
typedef unsigned char      HRINT8U;
typedef signed   char      HRINT8S;
typedef unsigned short     HRINT16U;
typedef signed   short     HRINT16S;
typedef unsigned long      HRINT32U;
typedef signed   long      HRINT32S;
typedef unsigned long long HRINT64U;
typedef signed long long   HRINT64S;
typedef float              HRFLOAT;              
typedef double             HRDOUBLE;              
typedef HRINT8U            HRBOOL;                 

typedef HRINT16U           HRINT16UI;
typedef HRINT16S           HRINT16US;

typedef volatile HRINT8U   HRINT8VU;
typedef volatile HRINT16U  HRINT16VU;
typedef volatile HRINT32U  HRINT32VU;

#endif
