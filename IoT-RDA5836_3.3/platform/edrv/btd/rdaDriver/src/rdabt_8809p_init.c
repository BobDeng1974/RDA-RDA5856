/* 
 * Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/



#ifdef __BT_RDABT__
#include "stdio.h"                  /* Basic C I/O functions */

#include "rdabt_drv.h"

#ifdef __RDA_CHIP_R19_8809P__

#define RDA_READ_UINT32( _register_ ) \
        (*((volatile unsigned int *)(_register_)))

#define RDA_WRITE_UINT32( _register_, _value_ ) \
        (*((volatile unsigned int *)(_register_)) = (_value_))


UINT32 BT_DATA_INTERNAL btcore_rf_init_19[][2] = 
{
{0xa16082fc,0X00000000},
{0xa1608204,0X00001FFF},
{0xa1608218,0X0000161C},
{0xa160821c,0X0000040D},
{0xa1608220,0X00008326},
{0xa1608224,0X000004B5},
{0xa160822c,0X0000238F},
{0xa1608230,0X000085E8},
{0xa1608238,0X00000920},
{0xa160823c,0X00008DB3},
{0xa1608240,0X00001400},
{0xa1608248,0X00005604},
{0xa1608250,0X00004ECC},
{0xa1608254,0X00005124},
{0xa1608260,0X00000812},
{0xa1608264,0X000010C8},
{0xa1608278,0X00003024},
{0xa160828c,0X00001111},
{0xa1608290,0X00002468},
{0xa160829c,0X00001111},
{0xa16082a0,0X00002358},
{0xa16082c8,0X00000E00},
{0xa16082fc,0X00000001},
{0xa1608228,0X00000018},
{0xa1608200,0X0000020f},
{0xa1608204,0X0000f9cf},
{0xa1608208,0X0000fc2f},
{0xa160820c,0X0000f92f},
{0xa1608210,0X0000fa2f},
{0xa1608214,0X0000fc2f},
{0xa1608218,0X0000fb3f},
{0xa160821c,0X00007fff},
{0xa160825c,0X0000E7F4},
{0xa1608260,0X0000F8F8},
{0xa1608264,0X0000FDFD},
{0xa1608268,0X0000F2F3},
{0xa160826c,0X0000FEFF},
{0xa1608270,0X0000FEFF},
{0xa1608274,0X0000FFFF},
{0xa1608278,0X0000FFFF},
{0xa160827c,0X0000FFFF},
{0xa1608288,0X00000E93},
{0xa1608294,0X000003E1},
{0xa1608298,0X000047A5},
{0xa16082a0,0x00002356},//0X00006800
{0xa16082b4,0X0000006A},
{0xa16082bc,0X00001100},
{0xa16082c8,0X000088FB},
{0xa16082fc,0x00000000},
{0xa1608304,0x0000000b},
{0xa1608308,0x000025bd},
{0xa160830c,0x0000bfc6},
{0xa1608310,0x0000040f},
{0xa1608324,0x000008e4},
{0xa16083a4,0x00004075},
{0xa16083ac,0x000010c0},
{0xa16082fc,0x00000001},
{0xa1608300,0x00002000},
//{0xa1608314,0x00003000},
{0xa1608314,0x00005fff},
{0xa160831c,0x0000d132},
{0xa1608324,0x00000008},
{0xa16082fc,0x00000000},
{0xa16082c0,0x00000129},
{0xa16082c0,0x0000012b},
	
};




const uint32 rda_pskey_19[][2] =
{
#ifdef __RDABT_DISABLE_EDR__

{0x80000470,0xf88dffff}, ///disable edr
#ifdef __RDABT_ENABLE_SP__
{0x80000474,0x83793818}, ///disable 3m esco ev4 ev5
#else
{0x80000474,0x83713818}, ///disable 3m esco ev4 ev5
#endif

#else // !__RDABT_DISABLE_EDR__

{0x80000470,0xfe8dffff}, ///enable edr

#ifdef __RDABT_ENABLE_SP__
{0x80000474,0x83793998}, ///disable 3m esco ev4 ev5
#else
{0x80000474,0x83713998}, ///disable 3m esco ev4 ev5
#endif

#endif // !__RDABT_DISABLE_EDR__

#ifdef BT_UART_BREAK_INT_WAKEUP
//{0x800000a4,0x0f20280a}, // use uart tx to wake host	
//{0x80000040,0x02000000},
{0x40200010,0x00007fff}, // set hostwake gpio to input
#else
//host wakeup
{0x40200010,0x00007f7f},
#endif
	{0x800004d0,0x007803fd}, // set buffer size
	{0x800004d4,0x00080006},
	{0x800004d8,0x007803fd},
	{0x800004dc,0x00080004},
	{0x800004e0,0x00010078},

};

// this function is called by bt_core when initialise
void BT_FUNC_INTERNAL RDABT_core_rf_Initialization_r19(void)
{
	int i;
	HWdelay_Wait_For_ms(50, FALSE);
	for(i=0;i<sizeof(btcore_rf_init_19)/sizeof(btcore_rf_init_19[0]); i++)
	{
		RDA_WRITE_UINT32(btcore_rf_init_19[i][0],btcore_rf_init_19[i][1]);
		HWdelay_Wait_For_ms(10, FALSE);
	}
	HWdelay_Wait_For_ms(20, FALSE);
}


const uint8 rdabt_pskey_sleep[] = {0xa, 0x10, 0x50, 0x01, 0xa, 0x8};
const uint8 rdabt_pskey_rf_setting[] = {0x00,0x0a,0x00,0x0c,0x40,0x30,0xb5,0x30,0xb5,0x30,0xba,0xba};
//const uint8 rdabt_pskey_pcm_config[] = {0x08,0xc0,0x18,0x10};

#ifdef __BT_PCM_OVER_UART__
const uint8 rdabt_pskey_sys_config[] = {0x80,0x20,0x08,0x00};
#else
const uint8 rdabt_pskey_sys_config[] = {0x00,0x20,0x08,0x00};
#endif
#ifdef USE_VOC_CVSD
const uint8 rdabt_pskey_pcm_config[] = {0x08,0xc0,0x98,0x90};
#else
const uint8 rdabt_pskey_pcm_config[] = {0x07,0xc0,0x98,0x90};	
#endif


void Rdabt_Pskey_Write_r19(void)
{
   int32 i;
   for(i=0;i<sizeof(rda_pskey_19)/sizeof(rda_pskey_19[0]);i++)
   {
      rdabt_wirte_memory(rda_pskey_19[i][0],&rda_pskey_19[i][1],1,0);
      RDABT_DELAY(1);
   }
 //   rdabt_write_pskey(0x35, rdabt_pskey_hostwake, sizeof(rdabt_pskey_hostwake));
 //   RDABT_DELAY(5);
//    rdabt_write_pskey(0x21, rdabt_pskey_sleep, sizeof(rdabt_pskey_sleep));
//    RDABT_DELAY(5);
    rdabt_write_pskey(0x15, rdabt_pskey_sys_config, sizeof(rdabt_pskey_sleep));
    RDABT_DELAY(5);
    rdabt_write_pskey(0x24, rdabt_pskey_rf_setting, sizeof(rdabt_pskey_rf_setting));
    RDABT_DELAY(5);
    rdabt_write_pskey(0x17, rdabt_pskey_pcm_config, sizeof(rdabt_pskey_pcm_config));
    RDABT_DELAY(5);
//   rdabt_write_pskey(0x25, rdabt_pskey_rssi_config, sizeof(rdabt_pskey_rssi_config));	
   
   
	
}

void RDABT_core_Intialization_r19(void)
{
    EDRV_TRACE(EDRV_BTD_TRC, 0, "RDABT_core_Intialization_r18");

    //RDABT_rf_Intialization_r19();
    Rdabt_Pskey_Write_r19();
}

#endif // __RDA_CHIP_r18_8809__

#endif // __BT_RDABT__
 

