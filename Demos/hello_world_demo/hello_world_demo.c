/*----------------------------------------------------------------------------
 * Copyright (c) <2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
/**
 *  DATE                AUTHOR      INSTRUCTION
 *  2019-07-23 10:00    yuhengP    The first version  
 *
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <osal.h>
#include <oc_lwm2m_al.h>
#include <link_endian.h>
#include "i2c.h"
#include "mlx90614.h"

#define cn_endpoint_id        "867725036001737"
#define cn_app_server         "119.3.250.80"
#define cn_app_port           "5683"

#define cn_app_Mlx90614 0x0

#pragma pack(1)
float    Tem;
typedef struct
{
    int8_t messageId;
    int16_t Tem;
} tag_app_Mlx90614;

#define cn_app_rcv_buf_len 128
static int             s_rcv_buffer[cn_app_rcv_buf_len];
static int             s_rcv_datalen;
static osal_semp_t     s_rcv_sync;


static int app_report_task_entry()
{
    int ret = -1;

    oc_config_param_t      oc_param;
    tag_app_Mlx90614 mlx90614info;

    (void) memset(&oc_param,0,sizeof(oc_param));

    oc_param.app_server.address = cn_app_server;
    oc_param.app_server.port = cn_app_port;
    oc_param.app_server.ep_id = cn_endpoint_id;
    oc_param.boot_mode = en_oc_boot_strap_mode_factory;
   // oc_param.rcv_func = app_msg_deal;

    ret = oc_lwm2m_config( &oc_param);
    if (0 != ret)
    {
    	return ret;
    }

    //install a dealer for the led message received
    while(1) //--TODO ,you could add your own code here
    {
        mlx90614info.messageId = cn_app_Mlx90614;
        mlx90614info.Tem = (int16_t)Tem;
        oc_lwm2m_report( (char *)&mlx90614info, sizeof(mlx90614info), 1000);
        osal_task_sleep(2*1000);
    }
    return ret;
}


static int app_mlx90614_entry()
{
    MX_I2C1_Init();
    Tem = 0;
    while (1)
    {
        Tem = MLX90614_ReadTemp(0x00,0x07);
		printf("tem:%f\n",Tem);
        osal_task_sleep(4*1000);
    }
}

int standard_app_demo_main()
{
    osal_task_create("mlx90614",app_mlx90614_entry,NULL,0x400,NULL,3);
    osal_task_create("report",app_report_task_entry,NULL,0x400,NULL,2);
    return 0;
}





