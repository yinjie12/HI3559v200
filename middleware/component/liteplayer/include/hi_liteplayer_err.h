/**
* Copyright (C), 2016-2020, Hisilicon Tech. Co., Ltd.
* All rights reserved.
*
* @file      hi_liteplayer_err.h
* @brief     hiLITEPLAYER module header file
* @author    HiMobileCam middleware develop team
* @date      2016.06.06
*/

#ifndef __HI_LITEPLAYER_ERR_H__
#define __HI_LITEPLAYER_ERR_H__

#include "hi_error_def.h"
#include "hi_defs.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*��0x40~0xFF*/
typedef enum hiAPP_LITEPLAYER_ERR_CODE_E
{
    APP_LITEPLAYER_ERR_INVALID_HANDLE  = 0x41,                   /**<handle is invalid*/
    APP_LITEPLAYER_ERR_ILLEGAL_HANDLE = 0x42,                      /**<handle is inllegal*/
    APP_LITEPLAYER_ERR_MEM_MALLOC = 0x43,                     /**<memery alloc failed*/
    APP_LITEPLAYER_ERR_THREAD_CREATE_FAIL = 0x44,            /**<thread create failed*/
    APP_LITEPLAYER_ERR_MUTEX_INIT_FAIL = 0x45,               /**<thread mutex init failed*/
    APP_LITEPLAYER_ERR_ADEC_IN_USE = 0x46,               /**<thread mutex init failed*/
    APP_LITEPLAYER_ERR_CALLBACK_ERROR = 0x47,                   /**< liteplayer callback failed*/
    APP_LITEPLAYER_ERR_ILLEGAL_STATE_ACTION = 0x48,            /**< error action in current state*/
    APP_LITEPLAYER_ERR_DEMUX_ERROR = 0x49,                 /**< exec demux error*/
    APP_LITEPLAYER_ERR_DEMUX_HAS_REG = 0x50,                 /**< has reg the demux lib*/
    APP_LITEPLAYER_ERR_NOT_SUPPORT = 0x51,                 /**< not support this action*/
    APP_LITEPLAYER_ERR_REINITED = 0x52,                    /**<liteplayer reinit*/
    APP_LITEPLAYER_ERR_NOT_INIT = 0x53,                    /**<liteplayer no init*/
    APP_LITEPLAYER_ERR_AVPLAYER = 0x54,                    /**<avplayer interface exec failed*/
    APP_LITEPLAYER_ERR_DLOPEN = 0x55,                    /**< dlopen failed*/
    APP_LITEPLAYER_ERR_DLSYM = 0x56,                    /**< dlsym failed*/
    APP_LITEPLAYER_ERR_NOT_REG_DEMUXER = 0x57,                    /**< not reg demuxer */
    APP_LITEPLAYER_ERR_HAS_CREATE = 0x58,                    /**< has create a liteplayer handle before */
    APP_LITEPLAYER_ERR_OTHER = 0x59,                    /**< unknow error */
    APP_LITEPLAYER_ERR_NDK = 0x60,                    /**< ndk error */
    APP_LITEPLAYER_ERR_DEMUX_IN_USE = 0x61,                 /**< the demux lib is used now */
    APP_LITEPLAYER_ERR_ADEC_NOTREG = 0x62,                 /**< the adec lib not register */
    APP_LITEPLAYER_ERR_ADEC_HAS_REG = 0x63,                 /**< the adec lib alread register */
    APP_LITEPLAYER_ERR_ADEC_ERR = 0x64,                 /**< create adec fail*/
    APP_LITEPLAYER_ERR_MAX_ERR = 0x65,                 /**< create player max fail*/
    APP_LITEPLAYER_ERR_MEM_SET = 0x66,                     /**<memery SET failed*/
    APP_LITEPLAYER_BUTT = 0xFF
} APP_LITEPLAYER_ERR_CODE_E;


/*general error code*/
#define HI_ERR_LITEPLAYER_NULL_PTR                            HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_ERR_NULL_PTR)
#define HI_ERR_LITEPLAYER_ILLEGAL_PARAM                       HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_ERR_ILLEGAL_PARAM)
#define HI_ERR_LITEPLAYER_ILLEGAL_HANDLE                       HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_ILLEGAL_HANDLE)
#define HI_ERR_LITEPLAYER_MUTEX_INIT_FAIL                     HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_MUTEX_INIT_FAIL)
#define HI_ERR_LITEPLAYER_ADEC_IN_USE                     HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_ADEC_IN_USE)
#define HI_ERR_LITEPLAYER_MEM_MALLOC                          HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_MEM_MALLOC)
#define HI_ERR_LITEPLAYER_THREAD_CREATE_FAIL                          HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_THREAD_CREATE_FAIL)
#define HI_ERR_LITEPLAYER_CALLBACK_ERROR                         HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_CALLBACK_ERROR)
#define HI_ERR_LITEPLAYER_ILLEGAL_STATE_ACTION                 HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_ILLEGAL_STATE_ACTION)
#define HI_ERR_LITEPLAYER_DEMUX_ERROR                       HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_DEMUX_ERROR)
#define HI_ERR_LITEPLAYER_DEMUX_HAS_REG                       HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_DEMUX_HAS_REG)
#define HI_ERR_LITEPLAYER_NOT_SUPPORT                       HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_NOT_SUPPORT)
#define HI_ERR_LITEPLAYER_REINITED                            HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_REINITED)
#define HI_ERR_LITEPLAYER_NOT_INIT                            HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_NOT_INIT)
#define HI_ERR_LITEPLAYER_AVPLAYER                            HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_AVPLAYER)
#define HI_ERR_LITEPLAYER_DLOPEN                            HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_DLOPEN)
#define HI_ERR_LITEPLAYER_DLSYM                            HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_DLSYM)
#define HI_ERR_LITEPLAYER_NOT_REG_DEMUXER                            HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_NOT_REG_DEMUXER)
#define HI_ERR_LITEPLAYER_HAS_CREATE                           HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_HAS_CREATE)
#define HI_ERR_LITEPLAYER_OTHER                           HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_OTHER)
#define HI_ERR_LITEPLAYER_NDK                           HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_NDK)
#define HI_ERR_LITEPLAYER_DEMUX_IN_USE                           HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_DEMUX_IN_USE)
#define HI_ERR_LITEPLAYER_ADEC_NOT_REG                           HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_ADEC_NOTREG)
#define HI_ERR_LITEPLAYER_ADEC_HAS_REG                           HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_ADEC_HAS_REG)
#define HI_ERR_LITEPLAYER_ADEC_ERROR                           HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_ADEC_ERR)
#define HI_ERR_LITEPLAYER_REACH_MAX                           HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_MAX_ERR)
#define HI_ERR_LITEPLAYER_MEM_SET                          HI_APP_DEF_ERR(HI_APPID_LOCALPLAYER,APP_ERR_LEVEL_ERROR,APP_LITEPLAYER_ERR_MEM_SET)




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_LITEPLAYER_ERR_H__ */
