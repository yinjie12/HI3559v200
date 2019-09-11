
/******************************************************************************

  Copyright (C), 2016, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : imx377_sensor_ctl.c

  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2013/11/07
  Description   :
  History       :
  1.Date        : 2013/11/07
    Author      :
    Modification: Created file

******************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <ctype.h>
//#include <linux/fb.h>
#include <sys/mman.h>
#include <memory.h>

//#include "hi_comm_video.h"
#include "hi_sns_ctrl.h"
#include "imx377_cmos_priv.h"

#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "hi_i2c.h"
//#include "drv_i2c.h"
#endif


const unsigned char imx377_i2c_addr     =    0x34;        /* I2C Address of imx377 */
const unsigned int  imx377_addr_byte    =    2;
const unsigned int  imx377_data_byte    =    1;
static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ... (ISP_MAX_PIPE_NUM - 1)] = -1};
static HI_BOOL g_bStandby[ISP_MAX_PIPE_NUM] = {0};

extern WDR_MODE_E genSensorMode;
extern HI_U8 gu8SensorImageMode;
extern HI_BOOL bSensorInit;
extern const IMX377_VIDEO_MODE_TBL_S g_astImx377ModeTbl[];

extern ISP_SNS_STATE_S   *g_pastImx377[ISP_MAX_PIPE_NUM];
extern ISP_SNS_COMMBUS_U  g_aunImx377BusInfo[];
#ifndef NA
#define NA 0xFFFF
#endif

int imx377_i2c_init(VI_PIPE ViPipe)
{
    int ret;
    HI_U8 u8DevNum;
    char acDevFile[16];

    if (g_fd[ViPipe] >= 0)
    {
        return HI_SUCCESS;
    }

    u8DevNum = g_aunImx377BusInfo[ViPipe].s8I2cDev;

    snprintf(acDevFile, sizeof(acDevFile),  "/dev/i2c-%u", u8DevNum);

    g_fd[ViPipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);

    if (g_fd[ViPipe] < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "Open /dev/hi_i2c_drv-%u error!\n", u8DevNum);
        return HI_FAILURE;
    }

    ret = ioctl(g_fd[ViPipe], I2C_SLAVE_FORCE, (imx377_i2c_addr >> 1));
    if (ret < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "I2C_SLAVE_FORCE error!\n");
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return ret;
    }

    return HI_SUCCESS;
}

int imx377_i2c_exit(VI_PIPE ViPipe)
{
    if (g_fd[ViPipe] >= 0)
    {
        close(g_fd[ViPipe]);
        g_fd[ViPipe] = -1;
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

int imx377_read_register(VI_PIPE ViPipe, int addr)
{
    return HI_SUCCESS;
}
int imx377_write_register(VI_PIPE ViPipe, int addr, int data)
{
    int ret;
    int idx = 0;
    char buf[8];

    if (0 > g_fd[ViPipe])
    {
        return HI_SUCCESS;
    }

    if (imx377_addr_byte == 2)
    {
        buf[idx] = (addr >> 8) & 0xff;
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    }
    else
    {
        buf[idx] = addr & 0xff;
        idx++;
    }

    if (imx377_data_byte == 2)
    {
        buf[idx] = (data >> 8) & 0xff;
        idx++;
        buf[idx] = data & 0xff;
        idx++;
    }
    else
    {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[ViPipe], buf, (imx377_addr_byte + imx377_data_byte));
    if (ret < 0)
    {
        ISP_TRACE(HI_DBG_ERR, "I2C_WRITE DATA error!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

void imx377_standby(VI_PIPE ViPipe)
{
    imx377_write_register(ViPipe, 0x3000, 0x5);
    g_bStandby[ViPipe] = HI_TRUE;
    return;
}

void imx377_restart(VI_PIPE ViPipe)
{
    imx377_write_register(ViPipe, 0x3000, 0x4);
    g_bStandby[ViPipe] = HI_FALSE;
    return;
}

//static void delay_ms(int ms)
//{
//    usleep(ms * 1000);
//}

static const HI_U16 gs_au16SensorCfgSeqDiff[][IMX377_MODE_BUTT + 1] =
{
    {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3123},
    {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3ac4},
    {0x19, 0x19, 0xb1, 0x31, 0x00, 0x14, 0x00, 0x10, 0x00, 0x3004},
    {0x01, 0x01, 0x25, 0x15, 0x01, 0x35, 0x07, 0x07, 0x01, 0x3005},
    {0x04, 0x04, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x3006},
    {0x00, 0x00, 0x00, 0x00, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x3039},
    {0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x303a},
    {0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x303e},
    {0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x303f},
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x3040},
    {0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x307f},
    {0x48, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30d0},
    {0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30d1},
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x30d5},
    {0x00, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x30d6},
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x30d7},
    {0x00, 0x00, 0x00, 0x00, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x30d8},
    {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x30d9},
    {0x00, 0x00, 0x02, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x30da},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x30ee},
    {0x20, 0x10, 0x08, 0x08, 0x04, 0x08, 0x94, 0xa0, 0x78, 0x30f5},
    {0x04, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x04, 0x30f6},
    {0xe3, 0xe3, 0xe3, 0xe3, 0x83, 0x8e, 0x30, 0xf7, 0x4e, 0x30f7},
    {0x08, 0x08, 0x08, 0x08, 0x04, 0x03, 0x0c, 0x0d, 0x0c, 0x30f8},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30f9},
    {0x86, 0x86, 0x4a, 0x4a, 0x0e, 0xe2, 0xf6, 0x86, 0xF6, 0x312f},
    {0x08, 0x08, 0x04, 0x04, 0x03, 0x02, 0x0b, 0x08, 0x0B, 0x3130},
    {0x7e, 0x7e, 0x46, 0x46, 0xfe, 0xde, 0xe6, 0x7e, 0xe6, 0x3131},
    {0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x0b, 0x08, 0x0B, 0x3132},
    {0x08, 0x08, 0x04, 0x04, 0x10, 0x04, 0x10, 0x08, 0x10, 0x3a41},
    {0x77, 0xa7, 0xa7, 0xa7, 0xa7, 0xA7, 0xa7, 0xa7, 0x77, 0x3133},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3134},
    {0x37, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x37, 0x3137},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3138},
    {0x67, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x67, 0x3135},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3136},
    {0x37, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x37, 0x313b},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x313c},
    {0x37, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x37, 0x3139},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x313a},
    {0x37, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x37, 0x313f},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3140},
    {0xdf, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0xdf, 0x313d},
    {0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x313e},
    {0x2f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x2f, 0x3141},
};

static const HI_U16 gs_au16SensorCfgSeq[][IMX377_MODE_BUTT + 1] =
{
    {0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xF0, 0xf0, 0xf0, 0xf0, 0x3120},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3121},
    {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x3122},
    {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3123},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3124},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x3125},
    {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x3127},
    {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x3129},
    {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x312a},
    {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x312d},
    {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3003},
    {0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x3045},
    {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x304e},
    {0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4A, 0x4a, 0x4a, 0x4a, 0x3057},
    {0xf6, 0xf6, 0xf6, 0xf6, 0xf6, 0xF6, 0xf6, 0xf6, 0xf6, 0x3058},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3059},
    {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x306b},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3145},
    {0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x3202},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3203},
    {0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x3236},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3237},
    {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0B, 0x0b, 0x0b, 0x0b, 0x3304},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3305},
    {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0B, 0x0b, 0x0b, 0x0b, 0x3306},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3307},
    {0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x337f},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3380},
    {0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 0x338d},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x338e},
    {0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x72, 0x3510},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3511},
    {0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0F, 0x0f, 0x0f, 0x0f, 0x3528},
    {0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0F, 0x0f, 0x0f, 0x0f, 0x3529},
    {0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0F, 0x0f, 0x0f, 0x0f, 0x352a},
    {0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0F, 0x0f, 0x0f, 0x0f, 0x352b},
    {0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0F, 0x0f, 0x0f, 0x0f, 0x3538},
    {0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x3539},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x353c},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3553},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3554},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3555},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3556},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3557},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3558},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3559},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x355a},
    {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x357d},
    {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x357f},
    {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x3580},
    {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x3583},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x3587},
    {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0B, 0x0b, 0x0b, 0x0b, 0x3590},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3591},
    {0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0F, 0x0f, 0x0f, 0x0f, 0x35ba},
    {0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0C, 0x0c, 0x0c, 0x0c, 0x366a},
    {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0B, 0x0b, 0x0b, 0x0b, 0x366b},
    {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x366c},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x366d},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x366e},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x366f},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3670},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3671},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3672},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3673},
    {0xdf, 0xdf, 0xdf, 0xdf, 0xdf, 0xDF, 0xdf, 0xdf, 0xdf, 0x3674},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3675},
    {0xa7, 0xa7, 0xa7, 0xa7, 0xa7, 0xA7, 0xa7, 0xa7, 0xa7, 0x3676},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x3677},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3687},
    {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x375c},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3799},
    {0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0A, 0x0a, 0x0a, 0x0a, 0x380a},
    {0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x382b},
    {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3ac4},
    {0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x303d},
    {0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x16, 0x3000},
    {0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xA2, 0xa2, 0xa2, 0xa2, 0x3018},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x3399},
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x310b},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a56},
    {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,     NA},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x310b},
    {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,     NA},
    {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x3000},
    {  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,     NA},
    {0x19, 0x19, 0xb1, 0x31, 0x00, 0x14, 0x00, 0x10, 0x00, 0x3004},
    {0x01, 0x01, 0x25, 0x15, 0x01, 0x35, 0x07, 0x07, 0x01, 0x3005},
    {0x04, 0x04, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x3006},
    {0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xA0, 0xa0, 0xa0, 0xa0, 0x3007},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3008},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x3009},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x300a},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x3011},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x302d},
    {0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x300b},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x300c},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x300d},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x300e},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x301a},
    {0x00, 0x00, 0x00, 0x00, 0xbc, 0x00, 0x00, 0x00, 0x00, 0x3039},
    {0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x303a},
    {0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x303e},
    {0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x303f},
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x3040},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3068},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x307e},
    {0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x307f},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3080},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3081},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3082},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3083},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3084},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3085},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3086},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3087},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3095},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3096},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3097},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3098},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3099},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x309a},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x309b},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x309c},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30bc},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30bd},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30be},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30bf},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c0},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c1},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c2},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c2},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c3},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c4},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c5},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c6},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c7},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c8},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30c9},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30ca},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30cb},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30cc},
    {0x48, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30d0},
    {0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30d1},
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x30d5},
    {0x00, 0x00, 0x00, 0x00, 0x74, 0x00, 0x00, 0x00, 0x00, 0x30d6},
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x30d7},
    {0x00, 0x00, 0x00, 0x00, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x30d8},
    {0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x30d9},
    {0x00, 0x00, 0x02, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x30da},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x30ee},
    {0x20, 0x10, 0x08, 0x08, 0x04, 0x08, 0x94, 0xa0, 0x78, 0x30f5},
    {0x04, 0x02, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x04, 0x30f6},
    {0xe3, 0xe3, 0xe3, 0xe3, 0x83, 0x8e, 0x30, 0xf7, 0x4e, 0x30f7},
    {0x08, 0x08, 0x08, 0x08, 0x04, 0x03, 0x0c, 0x0d, 0x0c, 0x30f8},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30f9},
    {0x86, 0x86, 0x4a, 0x4a, 0x0e, 0xe2, 0xf6, 0x86, 0xF6, 0x312f},
    {0x08, 0x08, 0x04, 0x04, 0x03, 0x02, 0x0b, 0x08, 0x0B, 0x3130},
    {0x7e, 0x7e, 0x46, 0x46, 0xfe, 0xde, 0xe6, 0x7e, 0xe6, 0x3131},
    {0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x0b, 0x08, 0x0B, 0x3132},
    {0x08, 0x08, 0x04, 0x04, 0x10, 0x04, 0x10, 0x08, 0x10, 0x3a41},
    {0x77, 0xa7, 0xa7, 0xa7, 0xa7, 0xA7, 0xa7, 0xa7, 0x77, 0x3133},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3134},
    {0x37, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x37, 0x3137},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3138},
    {0x67, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x67, 0x3135},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3136},
    {0x37, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x37, 0x313b},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x313c},
    {0x37, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x37, 0x3139},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x313a},
    {0x37, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x6f, 0x37, 0x313f},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3140},
    {0xdf, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0xdf, 0x313d},
    {0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x313e},
    {0x2f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x2f, 0x3141},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3142},
    {0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x47, 0x3a86},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a87},
    {0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x3143},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3144},
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x3001},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30f4},
};

void imx377_init(VI_PIPE ViPipe)
{
    HI_U32          i;
    HI_U8            u8ImgMode;
    HI_BOOL         bInit;

    bInit       = g_pastImx377[ViPipe]->bInit;
    u8ImgMode   = g_pastImx377[ViPipe]->u8ImgMode;
    HI_U8 u16RegData;
    HI_U16 u16RegAddr;
    HI_U32 u32SeqEntries;

    if (HI_FALSE == bInit)
    {
        /* 2. sensor i2c init */
        imx377_i2c_init(ViPipe);
    }

    if ((!g_bStandby[ViPipe]) || (!g_pastImx377[ViPipe]->bInit))
    {
        u32SeqEntries = sizeof(gs_au16SensorCfgSeq) / sizeof(gs_au16SensorCfgSeq[0]);
        for ( i = 0 ; i < u32SeqEntries; i++ )
        {
            u16RegAddr = gs_au16SensorCfgSeq[i][IMX377_MODE_BUTT];
            u16RegData = gs_au16SensorCfgSeq[i][u8ImgMode];

            if (NA == u16RegData)
            {
                //delay_ms(10);
                continue;
            }
            imx377_write_register(ViPipe, u16RegAddr, u16RegData);
        }
    }
    else
    {
        u32SeqEntries = sizeof(gs_au16SensorCfgSeqDiff) / sizeof(gs_au16SensorCfgSeqDiff[0]);
        for ( i = 0 ; i < u32SeqEntries; i++ )
        {
            u16RegAddr = gs_au16SensorCfgSeqDiff[i][IMX377_MODE_BUTT];
            u16RegData = gs_au16SensorCfgSeqDiff[i][u8ImgMode];

            if (NA == u16RegData)
            {
                //delay_ms(10);
                continue;
            }
            imx377_write_register(ViPipe, u16RegAddr, u16RegData);
        }
        imx377_restart(ViPipe);
    }

    for (i = 0; i < g_pastImx377[ViPipe]->astRegsInfo[0].u32RegNum; i++)
    {
        imx377_write_register(ViPipe, g_pastImx377[ViPipe]->astRegsInfo[0].astI2cData[i].u32RegAddr, g_pastImx377[ViPipe]->astRegsInfo[0].astI2cData[i].u32Data);
    }

    g_pastImx377[ViPipe]->bInit = HI_TRUE;

    printf("IMX377 %s init succuss!\n", g_astImx377ModeTbl[u8ImgMode].pszModeName);

    return ;

}

void imx377_exit(VI_PIPE ViPipe)
{
    imx377_i2c_exit(ViPipe);
    g_bStandby[ViPipe] = HI_FALSE;

    return;
}


