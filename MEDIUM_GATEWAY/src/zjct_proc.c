/****************************************************************************************************
* Copyright @,  2012-2017,  LCTX Co., Ltd. 
* Filename:     zjct_proc.c  
* Version:      1.00
* Date:         2012-04-20	    
* Description:  Z024��Яʽָ��ͨ���ն�II��ָ��ͨ�Ű���װ�׳�ͨ����ͨ��Э�鴦��
			<20120315_ZJCT-002װ�׳�ͨ����ͨ�Ź淶.doc> : II���ն��൱��ͨ��1�ų�Ա��
			�����ͳ�Ա�й���:
			1)��ͨ��zjct��ͨ������ֱ����067 INC�Բ��ŷ�ʽ���л���ͨ�ţ�
			2)��zjct��ͨ�������û����л���ͨ�š�
* Modification History: ��ʯ�� 2012-04-20 �½��ļ�

*****************************************************************************************************/
//application: zjct_proc.c
#ifndef __ZJCT_PROC_C__
#define __ZJCT_PROC_C__

#if defined(__cplusplus) 
extern "C" 
{ 
#endif

/* 
	״̬��ѯ��Ϣ�䵱����������յ�����Ϣ����±��ص����ֱ�־
   	FLG_OK - ���ֳɹ���FLG_FAILED - ����ʧ��(��ʼ����ֵ)
   	
��	�豸����			����
0	ָ�������Ŀ��ƺ�	0xF0
1	ս�������Ŀ��ƺ�	0xF0
2	��̨�ӿ��豸		0xF1
3	��Ա1�ź�			0xF4
4	��Ա2�ź�			0xF5
5	�û��ӿں�			0xF6
6	�û���չ��			0xF7
7	��Ա1�ź�			0xF8
8	xxx					0xF9
9	���ʹ㲥��ַ		0xFD
10	��ͨ�û���ַ		0xFE
11	��ͨ�����豸��ַ	0xFF
*/

/* includeͷ�ļ� */

#include "PUBLIC.h"
#include "zjct_proc.h"


/*��ģ��궨��*/
#define ZJCT_LOCAL_DEV_INDX 				0x08

#define STORE_FILE 						"/data/store.bin"
#define ZJCT_MEM_DATA_LEN					(sizeof(ZJCT_MEM))

#define NUMBER_OF_DEVICES_PER_DSP 		6
#define CY1HH_F400							(ZJCT_DEV_ADDR_CY1H << 8)
#define DTJKH_F100							(ZJCT_DEV_ADDR_DTJKH << 8)


#define ZJCT_DISCONNECT  					0x00
#define ZJCT_CONNECT						0x01

#define ZJCT_SET_OK						0x00
#define ZJCT_SET_NG						0x01


#undef 	INC_VOC_DEBUG
#define INC_VOC_DEBUG

#undef 	SEAT_CMD_DEBUG
#define SEAT_CMD_DEBUG


/*����ʱֻ��������״̬��������ʱ����
	ZJCT_LOCALMEM_STACK_CNT ��״̬*/
#undef 	SINGLE_MODE_710
#define SINGLE_MODE_710 


/* ����ģ��ȫ�ֱ����������� */


//static struct sockaddr_in gremoteinc_ip; /* INC IP ��ַ*/
//static struct sockaddr_in glocalinc_ip;  /* ��INC ͨ�ŵı���IP ��ַ*/
	
/* ��ģ��ȫ�ֱ������� */
#define AC491_REAL_DSP_CORE_CNT   2
#define AC491_VOC_CHAN_FOR_DIAL          	0x01
#define AC491_VOC_CHAN_FOR_ADD          	0x02
#define AC491_VOC_CHAN_FOR_RADIO       	AC491_VOC_CHAN_FOR_DIAL

#define AC491_MAX_CHAN_ALL	(NUMBER_OF_CHANNELS*AC491_REAL_DSP_CORE_CNT)

static uint8 g_chan[AC491_MAX_CHAN_ALL] = {0};	

extern void Send_TONE(unsigned short wport, TONE_TYPE tone_type);


/* ͨ����ͨ��������INC���к�������Ȼ���ͨ����socket */

static struct sockaddr_in g_ZjctIncCallSockAddr;  	//���ͺ��������INCʹ�õĵ�ַ,�˿ں�30000	
static struct sockaddr_in g_ZjctIncVocSockAddr;  	//���ͻ������ݸ�INCʹ�õĵ�ַ,�˿ں�30001	
static struct sockaddr_in g_ZjctAdptMngSockAddr;   	//���ͻ�������ͨ������ʹ�õĵ�ַ		
static struct sockaddr_in g_ZjctAdpterAddr;   		//���ͻ�������ͨ������ʹ�õĵ�ַ
static struct sockaddr_in g_ZjctAdptVocSockAddr;   	//���ͻ�������ͨ������ʹ�õĵ�ַ	
static struct sockaddr_in g_ZjctSeatMngSockAddr;		//Զ�˵�ϯλ�����socket��ַ

//static struct sockaddr_in g_ZjctRemoteSeatMngSockAddr;


//static struct sockaddr_in g_ZjctSeatDataSockAddr;
//static struct sockaddr_in g_ZjctSeatCallSockAddr;

/* �복ͨ����������ͨ����socket */
static int32 g_ZjctAdptMngSockFd	= 0 ; 	//�˿ں�0x6600
static int32 g_ZjctIncCallSockFd_ID[CHETONG_SEAT_MAX] = {0}; //��INC��������ͨ���ñ���socket ���
static int32 g_ZjctIncVocSockFd_ID[CHETONG_SEAT_MAX] 	= {0}; 	//��INC���л���ͨ���ñ���socket ���
static int32 g_ZjctAdptVocSockFd 	= 0; 	//�˿ں�0x6601
static int32 g_ZjctSeatMngSockFd 	= 0;
static int32 g_ZjctAdptMngSockFd_ID[CHETONG_SEAT_MAX] = {0};
static int32 g_ZjctAdptVocSockFd_ID[CHETONG_SEAT_MAX] = {0};

/* �豸�����ɹ���־����ʼ��Ϊ0,�������������ܵ�Ӧ�����1*/
uint8 gsend_register[CHETONG_SEAT_MAX] = {FLG_FAILED};
uint8 gnetwork_config 		= FLG_FAILED;
uint8 grequest_status [CHETONG_SEAT_MAX] = {FLG_FAILED};
uint8 gopenusermsg_status = FLG_FAILED;
uint8 gusernum_status 		= FLG_FAILED;
uint8 gsocketinit_status 	= FLG_FAILED;
int32 g_chan_auto_cfg = CHAN_NOCFG;
uint8 g_dbg_panel = 0;
uint32 time_cnt_100ms = 0; //�����Ա�Ƿ����ʱ��
uint8 gwork_mode = SEAT_MODE;				/*work mode for ZJCT or haifang*/

ETH_PORT_DES II_Seat_local;
ETH_PORT_DES II_Seat_remote;

ETH_PORT_DES II_Adpt_local[CHETONG_SEAT_MAX];
ETH_PORT_DES II_Adpt_remote;

ETH_PORT_DES II_Inc_local[CHETONG_SEAT_MAX];
ETH_PORT_DES II_Inc_remote;

uint32 ghw_ver = 0x01;
uint32 gsw_ver = 0x07;




uint8 gauto_send_cfg_flg = 0;
static uint8 g_groudcall_cnt = 0;

#define 	time_6s		300
static uint32 handle_time_cnt = 0;
static uint32 first_cnt = 0;
static uint32 danhu_cnt = 0;
uint32 gdanhu_timeout_flg = 0;
static uint32 online_mem = 0;
static uint32 online_mem_cnt = 0;
static uint32 online_radio = 0;
static uint32 online_radio_cnt = 0;

static uint8 radio_id[2] = {0};		/* ����ת�ŵ�̨��ַ����*/
static uint8 radio_ctl_type = 0; /* �����̨ת�ſ�������*/
static uint8 radio_ZX_type = 0; /* �����̨ת������*/
static uint8 radio_ZX_status = DTZX_STOP; /* �����̨ת��״̬*/

/* ���̻�����*/

/* �豸 ״̬��Ϣͬʱ���س�ͨ�ڲ��ĵ�������*/ 

/* ����INC ��Ա����*/
static ZJCT_INC_MEM_INFO glocal_incmem;

/* Զ��INC ��Ա����*/
//static ZJCT_INC_MEM_INFO gremote_incmem[LOCAL_INCMEM_NUM];


/* ���س�Աһ�źг�Ա����*/
static ZJCT_MEM glocal_mem_update[ZJCT_DEV_INDX_MAX] ;
#ifdef SINGLE_MODE_710
static ZJCT_MEM glocal_mem_save ;
#else
static ZJCT_MEM glocal_mem_stack[ZJCT_LOCALMEM_STACK_CNT];
#endif

//static uint8 gremote_mem_voc_data[VOICE_MEM_MAX][VOICE_MAX_LEN] = {{0}};
//static uint8 gremote_radio_voc_data[RADIO_NUM_MAX][VOICE_MAX_LEN] = {{0}};


/* Զ�˳�Աһ�źг�Ա����*/
static ZJCT_MEM gremote_mem_update[ZJCT_DEV_TYPE_MAX * ZJCT_DEV_INDX_MAX] ;
static ZJCT_MEM gremote_mem_save[ZJCT_DEV_TYPE_MAX * ZJCT_DEV_INDX_MAX] ;


/* Զ�˵�̨����*/
static ZJCT_RADIO_STATUS gremote_radio_update[RADIO_NUM_MAX];
static ZJCT_RADIO_STATUS gremote_radio_save[RADIO_NUM_MAX];


static uint8 adapter_voc_buf[ZJCT_MAX_DATA_LEN_600] = {0};
//static uint8 stack_cnt = 0;


uint8 panel_status = PANEL_OFFLINE;
static uint8 buzzer_status = FMQ_CTRL_OFF;


/* ״̬֡Ĭ��ֵ��ȥ֡ͷ5  ���ֽ�*/
static uint8 groupcall_mem[ZJCT_DEV_INDX_MAX] = 
{
	0x03,					/*group call member count*/
	0x01, 0x02, 0x03		/*group call member number*/
};	

static uint8 glocal_dev_default_data[ZJCT_STATUS_MSG_DATA_LEN] = 	
{
	0x0F, 
	ZJCT_LOCAL_DEV_INDX,	/*local device ID default value but machine 
								init will give new value*/ 
	0x02, 
	ZJCT_MODE_CNGB, 		/*local device ID default work mode*/
	0xFE, 0xFF, 			/*local device ID default send to address */
	
	0x00, 0x01,         	/* HW Version*/
	0x00, 0x01, 0x01, 0x06, /*Software Version*/ 

#if 1

	0x00,								/* phone number register flag byte*/
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* phone number */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

#elif 0

	0x00, 								/* phone number register flag byte*/
	0xef, 0xfc, 0xf0, 0xfc, 0xef, 0xfc, 0xef, 0xfc, /* phone number */
	0xf0, 0xfc, 0xef, 0xfc, 0xee, 0xfc, 0xef, 0xfc
	
#elif 0

	0x01,
	0x07, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x03, /*phone number 2002003*/
	0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,0x00
	
#endif

};

/* ���غ������� */
#if 0
static int32 Zjct_RpcDbg_Socket_init(void);
static int32 Zjct_RpcDbg_Socket_Close(void);
static int32 Zjct_Ac491_Loopback_Test(uint16 btimeslot, uint8 *p, uint32 len);
static int32 Zjct_Dbg_ReadOncePlayVoc(void);
static int32 Zjct_Dbg_RecordVoc(uint8 *buf);
static int32 Zjct_Dbg_PlayVoc(void);
static int32 Zjct_Msb_To_Lsb(uint8 *buf, uint32 len);
static int32 Zjct_Mainboard_To_Ac491(uint8 *pData, uint32 len);

/*
static int32 Zjct_Call_UnRegister_UserPhoneNumber_Ack(uint8 *buf, uint32 len);
static int32 Zjct_Call_Register_UserPhoneNumber(void);
static int32 Zjct_Call_GetUserInfoCfgMsg(uint8 *buf, uint32 len);
*/

//static int32 Zjct_SeatMng_Socket_init(void);

//static int32 Zjct_SeatCall_Socket_init(void);
//static int32 Zjct_SeatCall_Socket_Close(void);
static int32 Zjct_AdptMng_ParamUpdateMsgProc( uint8 *buf, uint32 len);
static int32 Zjct_AdptMng_ReportStatusInfoProc( uint8 *buf, uint32 len);
#endif

static int32 Zjct_AdptMng_Receive_Param_Set(uint8 *buf, uint32 len);
static int32 Zjct_AdptMng_SendTo_Param_Set_Ack(uint8 *buf, uint16 Ack_Len);
static int32 Zjct_AdptMng_SendParamConfig_Proc(uint8 *buf, uint32 len, uint8 id);
static int32 Zjct_AdptMng_SendParamConfig_Ack(uint8 *buf, uint32 len);
static int32 Zjct_AdptMng_SendRegisterNetwork_Req_Ack( uint8  *buf, uint32 len);
static int32 Zjct_AdptMng_Msg_Process(uint8 *buf, uint32 len, uint8 id, uint32 Ipaddr);
static int32 Zjct_AdptMng_Update_Mem1List_OnLine_Msg(uint8 * buf, uint32 len, uint32 Ipaddr );
static int32 Zjct_Socket_Send(int bsockfd, struct sockaddr_in *pt, uint8 *ptr, uint32 len);
//static int32 Zjct_SeatData_Socket_Send(uint8 *buf, uint32 len);
//static int32 Zjct_SeatCall_Socket_Send(uint8 *buf, uint32 len);
static int32 Zjct_AdptMng_Socket_Send(uint8 *buf, uint32 len, uint8 id);
static int32 Zjct_Sendto_Adapter_Socket(uint8 *buf, uint32 len, uint8 id);
static int32 Zjct_AdptMng_AutoSend_Close_GroupCall(void);
static int32 Zjct_Set_Bit(uint8 *buf, uint8 bit);
static int32 Zjct_Clean_Bit(uint8 *buf, uint8 bit);
static int32 free_one_channel(int32 chan);
//static int32 find_one_channel_for_radio(uint16 dev_id);
static int32 find_one_channel_for_member(void);



static int32 Zjct_Socket_init(void);

/*socket */
static int32 Zjct_IncCall_Socket_Close(uint8 id);
static int32 Zjct_IncVoc_Socket_Close(uint8 id);
static int32 Zjct_SeatMng_Socket_Close(void);

//static int32 Zjct_SeatData_Socket_init(void);
//static int32 Zjct_SeatData_Socket_Close(void);

static int32 Zjct_AdptVoc_Socket_init(void);
static int32 Zjct_AdptVoc_Socket_Close(void);
static int32 Zjct_AdptMng_Socket_init(void);
static int32 Zjct_AdptMng_Socket_Close(void);
static int32 Zjct_Conference_Mainboard_To_Ac491(uint32 channel, uint8 *buf, uint32 len);
static int32 Zjct_Terminal_Clash_SendTo_PC(uint8 id);

/*�����ⲿ��������*/
extern  int Ac491OpenAndActivate3WayChannel(int device, int channel);
extern int Ac491NetSideSwitchTdmChannel(int device, int channel1, int channel2);

/* shortת����1��char */
static int32 Buzzer_Control(int b_st)
{
    if(b_st > FMQ_CTRL_BUTT)
    {
        return DRV_ERR;
    }
    
	return DRV_OK;
}

static uint32 Linear2Alaw(uint8 *linearBuff, uint8 *aLawBuff, uint32 BuffLen)
{
	unsigned int i;
	
	for(i=0; i< (BuffLen/2); i++ )
	{
		/* shortת����1��char */
		aLawBuff[i] = (*((short *)linearBuff)) & 0xFF;
		linearBuff += 2;	
	}
	
	return i;
}



static int32 voc_swap(uint8 *buf, uint32 len)
{
	uint8 temp = 0;
	
	if(2 == len)
	{
		temp = buf[0];
		buf[0] = buf[1];
		buf[1] = temp;
	}

	return DRV_OK;
};

uint8 voc_add_temp_buf[ZJCT_VOC_PCM_20MS_LEN_320] = {0};

static int32 voc_add(uint8 *sum_buf, uint8 *buf, uint32 len)
{

	uint32 i = 0;
	int32 temp_sum = 0;

	memcpy(&voc_add_temp_buf[0], buf, len);
	
	for(i = 0; i < len; i+= 2)
	{

		voc_swap(&voc_add_temp_buf [i], 2);
#if 1		
		temp_sum = *((int16 *)(sum_buf + i));
		temp_sum  += *((int16 *)(&voc_add_temp_buf[i]));
		 if(temp_sum > 32760)
		 {
			*((int16 *)(sum_buf + i)) = 32760;
			ERR("too big %d\r\n", temp_sum);
		 }
		else if(temp_sum < -32760)
		 {
			*((int16 *)(sum_buf + i)) = -32760;
			ERR("too small %d\r\n", temp_sum);
		 }
		 else
		{
			*((int16 *)(sum_buf + i))  = temp_sum & 0xffff ;
		}
#else

		*((int16 *)(sum_buf + i))  = *((int16 *)(&voc_add_temp_buf[i])) ;

#endif
	}

	return DRV_OK;
}



/* member 0 ~ max*/
static int32 get_320_byte_from_mem(uint8 member, uint8 *buf, uint32 len)
{
	memcpy(buf, &gremote_mem_update[member].mem_voc[gremote_mem_update[member].mem_get][0],\
			len);
	gremote_mem_update[member].mem_get = (gremote_mem_update[member].mem_get + 1)%VOICE_TIME_1min;

	return len;
}

int32 Zjct_AdptVoc_SendTo_AC491_Save(void)
{
	uint16 dev_type = 0, dev_idx = 0, index =0;
						/* 0 ~ 31 */
	uint8 temp_buf[ZJCT_VOC_PCM_20MS_LEN_320] = {0};
	uint32 sum_len = ZJCT_VOC_PCM_20MS_LEN_320;
	uint8 sum_buf[ZJCT_VOC_PCM_20MS_LEN_320] = {0};
	uint8 alawbuf[ZJCT_VOC_G711_20MS_LEN_160] = {0};
	uint16 abuf_len = 0;

	dev_type = CY1HH_TYPE_IDX;
	for(dev_idx = 0; dev_idx < ZJCT_DEV_INDX_MAX; dev_idx++)
	{
		index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

		if((0xFF == gremote_mem_update[index].mem_chan_id)\
			&&(1 == gremote_mem_update[index].online))
		{
			if(gremote_mem_update[index].mem_save != gremote_mem_update[index].mem_get)
			{
				get_320_byte_from_mem(index, temp_buf, sum_len);
				voc_add(sum_buf, temp_buf, sum_len);
			}
		}
	}
	
	dev_type = CY2HH_TYPE_IDX;
	for(dev_idx = 0; dev_idx < ZJCT_DEV_INDX_MAX; dev_idx++)
	{
		index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

		if((0xFF == gremote_mem_update[index].mem_chan_id)\
			&&(1 == gremote_mem_update[index].online))
		{
			if(gremote_mem_update[index].mem_save != gremote_mem_update[index].mem_get)
			{
				get_320_byte_from_mem(index, temp_buf, sum_len);
				voc_add(sum_buf, temp_buf, sum_len);
			}
		}
	}

#if 0
	for(index = 0; index < RADIO_NUM_MAX; index++)
	{
		if(1 == gremote_radio_update[index].online)
		{
			if(gremote_radio_update[index].radio_save != gremote_radio_update[index].radio_get)
			{
				get_320_byte_from_mem(index, temp_buf, sum_len);
				voc_add(sum_buf, temp_buf, sum_len);
			}
		}
	}	
#endif

	abuf_len = Linear2Alaw(sum_buf, alawbuf, sum_len);
	if(ZJCT_VOC_G711_20MS_LEN_160 == abuf_len)
	{
		Zjct_Conference_Mainboard_To_Ac491(AC491_VOC_CHAN_FOR_ADD, alawbuf, abuf_len);
	}

	return DRV_OK;	
}


int32 Zjct_AdptVoc_Msg_Process_Add(uint8 *buf, uint32 len)
{
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;
	uint16 dev_type = 0, dev_idx = 0, index =0;

	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}
	
	switch(msg->stHead.ucSrcAddr)  
	{
		case ZJCT_DEV_ADDR_CY1H:
			if(msg->ucData[1] > ZJCT_DEV_INDX_MAX)
			{
				return DRV_ERR;
			}	
			
			dev_type = CY1HH_TYPE_IDX;
			dev_idx = msg->ucData[1] - 1;			/* 0 ~ 31 */
			index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

			if(1 == gremote_mem_update[index].online)
			{		
			#if 0
				if(gremote_mem_update[index].mem_save >= VOICE_TIME_1min)
				{
				ERR("%s: overrun \r\n", __func__);
					gremote_mem_update[index].mem_save = 0;
				}
				
				offset = gremote_mem_update[index].mem_save * ZJCT_VOC_PCM_20MS_LEN_320;
				memcpy(&gremote_mem_voc_data[dev_idx][offset],\
						&msg->ucData[3], ZJCT_VOC_PCM_20MS_LEN_320);
				gremote_mem_update[index].mem_save += 1;
			#else 
				memcpy(&gremote_mem_update[index].mem_voc[gremote_mem_update[index].mem_save][0],\
						&msg->ucData[3], ZJCT_VOC_PCM_20MS_LEN_320);	
				gremote_mem_update[index].mem_save = (gremote_mem_update[index].mem_save + 1) % VOICE_TIME_1min;		
			#endif
			}
			break;
		case ZJCT_DEV_ADDR_DTJKH:
			if(msg->ucData[1] > RADIO_NUM_MAX)
			{
				return DRV_ERR;
			}
			
			index = msg->ucData[1] - 1;
			if(1 == gremote_radio_update[index].online)
			{
			#if 0			
				if(gremote_radio_update[index].radio_save >= VOICE_TIME_1min)
				{
					gremote_radio_update[index].radio_save = 0;
				}
				offset = gremote_radio_update[index].radio_save * ZJCT_VOC_PCM_20MS_LEN_320;
				memcpy(&gremote_radio_voc_data[index][offset],\
						&msg->ucData[3], ZJCT_VOC_PCM_20MS_LEN_320);
				gremote_radio_update[index].radio_save += 1;
			#else
				memcpy(&gremote_radio_update[index].radio_voc[gremote_radio_update[index].radio_save][0],\
						&msg->ucData[3], ZJCT_VOC_PCM_20MS_LEN_320);	
				gremote_radio_update[index].radio_save = (gremote_radio_update[index].radio_save + 1) % VOICE_TIME_1min;
			#endif
			}
			break;
		case ZJCT_DEV_ADDR_YHKZH:
			break;
		default:
			ERR("Zjct_AdptVoc_Msg_Process param error"
				"msg->stHead.ucSrcAddr = 0x%x\r\n", msg->stHead.ucSrcAddr);
			return DRV_ERR;
			break;
	}

	return DRV_OK;
}	


/*******************************************************************************
* Function: Zjct_Socket_Send
*
* Description: ���ڷ��͵Ļ�������
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
static int32 Zjct_Socket_Send(int bsockfd, struct sockaddr_in *pt, uint8 *ptr, uint32 len)
{	
	int32 retval = 0;
	
	if ((0 == bsockfd)||(NULL == pt)||(NULL == ptr)||(0 == len))
	{
		/* ��ʼ�����ӡ������Ϣ����ȡ��*/
/*
		ERR("zjct socket send param invalid. bsockfd = %d,  len = %d \r\n",\
				bsockfd, len);
*/
		return DRV_ERR;
	}	
	
	retval = sendto(bsockfd, ptr, len, 0, (struct sockaddr *)pt, sizeof(struct sockaddr_in));
	if (DRV_ERR == retval)
	{
		ERR("Zjct_Socket_Send error(%s)!\r\n", inet_ntoa(pt->sin_addr));
		return DRV_ERR;
	}

	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_SeatMng_Socket_Send
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
int32 Zjct_SeatMng_Socket_Send(uint8 *buf, uint32 len)
{
	int32 retval = 0;
	
	retval = Zjct_Socket_Send(g_ZjctSeatMngSockFd, &g_ZjctSeatMngSockAddr, \
							buf, len);
	if(DRV_OK != retval)
	{
		ERR("Zjct_SeatMng_Socket_Send failed \r\n");
		return DRV_ERR;
	}

					
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdptVoc_Socket_Send
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
int32 Zjct_AdptVoc_Socket_Send(uint8 *buf, uint32 len)
{
	int32 retval = 0;

	if(FLG_OK == GET_SOCKTINIT_FLG())
	{
		retval = Zjct_Socket_Send(g_ZjctAdptVocSockFd, &g_ZjctAdptVocSockAddr, \
								buf, len);
		if(DRV_OK != retval)
		{
			ERR("Zjct_AdptVoc_Socket_Send failed \r\n");
			return DRV_ERR;
		}

	
	}
	
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdptMng_Socket_Send
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
static int32 Zjct_AdptMng_Socket_Send(uint8 *buf, uint32 len, uint8 id)
{
	int32 retval = 0;
	retval = Zjct_Socket_Send(g_ZjctAdptMngSockFd_ID[id], &g_ZjctAdptMngSockAddr, \
							buf, len);
	if(DRV_OK != retval)
	{
		ERR("%s failed \r\n", __func__);
		return DRV_ERR;
	}


	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdptMng_Socket_Send
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/

static int32 Zjct_Sendto_Adapter_Socket(uint8 *buf, uint32 len, uint8 id)
{
	int32 retval = 0;
	retval = Zjct_Socket_Send(g_ZjctAdptMngSockFd_ID[id], &g_ZjctAdpterAddr, \
							buf, len);
	if(DRV_OK != retval)
	{
		ERR("%s failed \r\n", __func__);
		return DRV_ERR;
	}

	
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_IncCall_Socket_Send
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
int32 Zjct_IncCall_Socket_Send(uint8 *buf, uint32 len, uint8 id)
{
	int32 retval = 0;
	retval = Zjct_Socket_Send(g_ZjctIncCallSockFd_ID[id], &g_ZjctIncCallSockAddr, \
							buf, len);
	if(DRV_OK != retval)
	{
		ERR("Zjct_IncCall_Socket_Send failed \r\n");
		return DRV_ERR;
	}

	
	return DRV_OK;
}
/*******************************************************************************
* Function: Zjct_IncVoc_Socket_Send
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
int32 Zjct_IncVoc_Socket_Send(uint8 *buf, uint32 len, uint8 id)
{
	int32 retval = 0;
	static uint8 i = 0;
		
	retval = Zjct_Socket_Send(g_ZjctIncVocSockFd_ID[id], &g_ZjctIncVocSockAddr, \
							buf, len);
	if(DRV_OK != retval)
	{
		ERR("Zjct_IncVoc_Socket_Send failed \r\n");
		return DRV_ERR;
	}

	i++;
	if(200 == i)
	{
		i = 0;
#if 0
		uint8 j = 0;
		printf("222%d:::", len);
		
		for(j = 0; j < 10; j++)
		{
			printf("0x%x:", buf[j]);
		}
		putchar('\n');
#endif
	}
	
	return DRV_OK;
}

/*******************************************************************************
* Function: ncCall_
*
* Description: ����ͨ����������������
*
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments:  ��������ʱ���þ��g_ZjctIncCallSockFd	����������
*******************************************************************************/
int32 BoardSem_CallAck_busy(uint8 *buf, int32 len, uint8 id)
{
	ST_BOARD_SEM  Send;

	printf("%s: \r\n", __func__);
	
	memset(&Send, 0x00, sizeof(Send));

	Send.Head.ProType = 0x01;
	Send.Head.Edition = 0x01;
	Send.Head.SemType = 0x02;
	Send.Head.Len = 0x05;

	Send.Data[0] = 0x02;
	Send.Data[1] = 0x03;
	Send.Data[2] = buf[7];
	Send.Data[3] = 0x01;
	Send.Data[4] = 0xFF;

	//Zjct_IncCall_Socket_Send((uint8 *)&Send, (Send.Head.Len+sizeof(ST_BOARD_SEM_HEAD)), id);
	
	return DRV_OK;
}

int32 BoardSem_Call_HandUp(uint8 *buf, int32 len, uint8 id)
{
	ST_BOARD_SEM  Send;

	printf("%s: \r\n", __func__);
	
	memset(&Send, 0x00, sizeof(Send));

	Send.Head.ProType = 0x01;
	Send.Head.Edition = 0x01;
	Send.Head.SemType = 0x02;
	Send.Head.Len = 0x05;

	Send.Data[0] = 0x04;
	Send.Data[1] = 0x03;
	Send.Data[2] = glocal_mem_update[id].glocaldev_num;
	Send.Data[3] = 0x01;
	Send.Data[4] = 0x01;

	//Zjct_IncCall_Socket_Send((uint8 *)&Send, (Send.Head.Len+sizeof(ST_BOARD_SEM_HEAD)), id);
	
	return DRV_OK;
}


/*******************************************************************************
* Function: Zjct_Ptt_Process
*
* Description: �ѵ�ǰϵͳװ�׳�ͨ��״̬��������
*
* Inputs: @buf: input buffer 
		@len: input buffer length
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/

int32 Zjct_Ptt_Send_Inc(uint8 ptt_flg, uint8 id)
{
	ST_BOARD_SEM  Send;

	printf("%s: \r\n", __func__);
	
	memset(&Send, 0x00, sizeof(Send));

	Send.Head.ProType = 0x01;
	Send.Head.Edition = 0x01;
	Send.Head.SemType = 0x02;
	Send.Head.Len = 0x04;

	Send.Data[0] = 0x0A;
	Send.Data[1] = 0x02;
	Send.Data[2] = glocal_mem_update[id].glocaldev_num;
	Send.Data[3] = ! ptt_flg;
	
	//Zjct_IncCall_Socket_Send((uint8 *)&Send, (Send.Head.Len + sizeof(ST_BOARD_SEM_HEAD)), id);
	
	return DRV_OK;
}

int32 Zjct_Ptt_Process(uint8 flag, uint8 id)
{
	if(DTJKH_F100 == (glocal_mem_update[id].data.dev_dscId & 0xff00))
	{
 		switch(glocal_mem_update[id].data.dev_mode)
		{
			case ZJCT_MODE_KZDTFS:
			case ZJCT_MODE_KZDTJS:
				
				if(PTT_DOWN == flag)
				{
					glocal_mem_update[id].data.dev_mode = ZJCT_MODE_KZDTFS;
					DBG("ZJCT_MODE_KZDTFS PTT Control send voice data\r\n");
				}
				else if(PTT_UP == flag)
				{
					glocal_mem_update[id].data.dev_mode = ZJCT_MODE_KZDTJS;
					glocal_mem_update[id].radio_ctl_status = RADIO_CTL_DISCONNECT;
					DBG("ZJCT_MODE_KZDTFS PTT Control receive voice data\r\n");
				}
				else
				{
					ERR("Zjct_Ptt_Process parma error flag = %d\r\n", flag);
				}

				break;
			case ZJCT_MODE_GTFS:
			case ZJCT_MODE_GTJS:

				if(PTT_DOWN == flag)
				{
					glocal_mem_update[id].data.dev_mode = ZJCT_MODE_GTFS;
					DBG("ZJCT_MODE_GTFS PTT Control send voice data\r\n");
				}
				else if(PTT_UP == flag)
				{
					glocal_mem_update[id].data.dev_mode = ZJCT_MODE_GTJS;
					glocal_mem_update[id].radio_gtctl_status = RADIO_CTL_DISCONNECT;
					DBG("ZJCT_MODE_GTJS PTT Control receive voice data\r\n");
				}
				else
				{
					ERR("Zjct_Ptt_Process parma error flag = %d\r\n", flag);
				}

				break;
			default:
				ERR("PTT down or up nothing to do \r\n");
				break;
		}
	}

	switch(glocal_mem_update[id].data.dev_mode)
	{
		case ZJCT_MODE_BHFS:
		case ZJCT_MODE_BHJS:
			//Zjct_Ptt_Send_Inc(flag);
			break;
		default:
			break;
	}
		
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_Local_Mem_Data_Push
*
* Description: �ѵ�ǰϵͳװ�׳�ͨ��״̬��������
*
* Inputs: @buf: input buffer 
		@len: input buffer length
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/

static int32 Zjct_Local_Mem_Data_Push(uint8 *buf, uint32 len)
{

	DBG("\r\n");
	DBG("Zjct_Local_Mem_Data_Push\r\n");

    if((NULL == buf)||(0 == len))
    {
        ERR("%s: param error\r\n", __func__);    
        return DRV_ERR;
    }
    
	//memset(&UpdateData, 0x00, sizeof(UpdateData));


#ifdef SINGLE_MODE_710
	ZJCT_MEM *UpdateData = NULL;	
	UpdateData = (ZJCT_MEM *)buf;

	//memcpy(&glocal_mem_save.data, buf, len); 
	glocal_mem_save.data.dev_mode = UpdateData->data.dev_mode;
	glocal_mem_save.data.dev_dscId = UpdateData->data.dev_dscId;	

#elif 0

	if((stack_cnt >= 0)&&(stack_cnt < ZJCT_LOCALMEM_STACK_CNT))
	{

		memcpy(&glocal_mem_stack[stack_cnt], buf, len); 
		stack_cnt++;
	}
	else
	{
		ERR("Zjct_Local_Mem_Data_Push error push_cnt = %d\r\n", stack_cnt);
		return DRV_ERR;
	}
#else


	memcpy(&glocal_mem_stack[0], buf, len); 


#endif
	
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_Local_Mem_Data_Pop
*
* Description: ��֮ǰϵͳװ�׳�ͨ��״̬�ָ�
*
* Inputs:  @buf: output buffer
		@len :  output buffer length
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
static int32 Zjct_Local_Mem_Data_Pop(uint8 *buf, uint32 len)
{
	DBG("\r\n");
	DBG("Zjct_Local_Mem_Data_Pop \r\n");
	
    if((NULL == buf)||(0 == len))
    {
        ERR("%s: param error\r\n", __func__);    
        return DRV_ERR;
    }
    
#ifdef SINGLE_MODE_710 

	ZJCT_MEM *UpdateData = NULL;	
	UpdateData = (ZJCT_MEM *)buf;
	
	if(ZJCT_MODE_KZDTJS == glocal_mem_save.data.dev_mode)
	{
		;
	}
	else
	{
		glocal_mem_save.data.dev_mode = ZJCT_MODE_CNGB;
		glocal_mem_save.data.dev_dscId  = ZJCT_BROADCASE_ADDR;
	}

	//memcpy(buf, &glocal_mem_save.data, len); 
	
	UpdateData->data.dev_mode = glocal_mem_save.data.dev_mode ;
	UpdateData->data.dev_dscId = glocal_mem_save.data.dev_dscId ;
	
#elif 0

	if((stack_cnt >= 0 + 1)&&(stack_cnt < ZJCT_LOCALMEM_STACK_CNT + 1 ))
	{
		stack_cnt--;
		memcpy(buf, &glocal_mem_stack[stack_cnt], len); 
	}
	else
	{
		ERR("Zjct_Local_Mem_Data_Pop error push_cnt = %d\r\n", stack_cnt);
		return DRV_ERR;
	}	
#else

	if((ZJCT_MODE_DTJS == glocal_mem_stack.data.dev_mode)
		||(ZJCT_MODE_DTFS == glocal_mem_stack.data.dev_mode))
	{
		glocal_mem_stack[0].data.dev_mode = ZJCT_MODE_CNGB;
		glocal_mem_stack[0].data.dev_dscId = ZJCT_BROADCASE_ADDR;
	}
	else
	{
		glocal_mem_stack[0].data.dev_mode = ZJCT_MODE_CNGB;
		glocal_mem_stack[0].data.dev_dscId = ZJCT_BROADCASE_ADDR;
	}

	memcpy(buf, &glocal_mem_stack[0], len); 
		
						
#endif

	return DRV_OK;
}


static int32 Zjct_Ac491Voc_SendTo_Inc(uint8 *buf, uint32 len, uint8 id)
{
	ZJCT_INCCALL_VOC_MSG msg;

	memset(&msg, 0, sizeof(msg));

	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}	

	msg.Head.ucProType = ZJCT_PRO_TYPE;
	msg.Head.ucProVer  = ZJCT_PRO_VER;
	msg.Head.ucSemType = ZJCT_INC_VOICE_DATA;
	msg.Head.usLen = 1 + ZJCT_VOC_G711_20MS_LEN_160;
	
	msg.inc_vocdata[0] = glocal_mem_update[id].glocaldev_num;

	memcpy(&msg.inc_vocdata[1], buf + RTP_HEADER_SIZE, ZJCT_VOC_G711_20MS_LEN_160);

/*			
	Zjct_IncVoc_Socket_Send((uint8 *)&msg, msg.Head.usLen + \
								sizeof(ST_ZJCT_CALL_SEM_HEADER));
*/
	return DRV_OK;
}
static int32 Zjct_Ac491Voc_SendTo_Group(uint8 *buf, uint32 len)
{
#if 0
	ST_ZJCT_MNG_MSG msg;
	int32 retval = 0;
	uint32 msg_len = 0;

	memset(&msg, 0, sizeof(msg));

	msg.stHead.ucType = ZJCT_MNG_TYPE_PCM128K;
	msg.stHead.ucDstAddr = ZJCT_USER_ADDR;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.usLen = ZJCT_VOC_PCM_20MS_LEN_320  + 3 ;  //��323�ֽڴ�����
	
	msg.ucData[0] = 0xFF;
	msg.ucData[1] = glocal_mem_update[id].glocaldev_num;
	msg.ucData[2] = 0x00;							/*�����ֽ�*/

	retval = Pcm_Voice_Alaw2Linear(buf + RTP_HEADER_SIZE, \
				ZJCT_VOC_G711_20MS_LEN_160, adapter_voc_buf);

	if(ZJCT_VOC_PCM_20MS_LEN_320 != retval)
	{
		ERR("Pcm_Voice_Alaw2Linear error retval  = %d !\r\n", retval);
		return DRV_ERR;
	}

	memcpy(&msg.ucData[3], adapter_voc_buf, ZJCT_VOC_PCM_20MS_LEN_320);
	
	msg_len = msg.stHead.usLen + sizeof(ST_ZJCT_MNG_HEADER);
	Zjct_AdptVoc_Socket_Send((uint8 *)&msg, msg_len);
	
#endif	
	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}
	
	return DRV_OK;
}
static int32  Zjct_Ac491Voc_SendTo_Adapter(uint8 *buf, uint32 len)
{
#if 0
	ST_ZJCT_MNG_MSG msg;
	int32 retval = 0;
	uint32 msg_len = 0;

	memset(&msg, 0, sizeof(msg));

	msg.stHead.ucType = ZJCT_MNG_TYPE_PCM128K;
	msg.stHead.ucDstAddr = ZJCT_USER_ADDR;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.usLen = ZJCT_VOC_PCM_20MS_LEN_320  + 3 ;  //��323�ֽڴ�����
	
	msg.ucData[0] = 0xFF;
	msg.ucData[1] = glocal_mem_update[id].glocaldev_num;
	msg.ucData[2] = 0x00;							/*�����ֽ�*/

	retval = Pcm_Voice_Alaw2Linear(buf + RTP_HEADER_SIZE, \
				ZJCT_VOC_G711_20MS_LEN_160, adapter_voc_buf);

	if(ZJCT_VOC_PCM_20MS_LEN_320 != retval)
	{
		ERR("Pcm_Voice_Alaw2Linear error retval  = %d !\r\n", retval);
		return DRV_ERR;
	}

	memcpy(&msg.ucData[3], adapter_voc_buf, ZJCT_VOC_PCM_20MS_LEN_320);
	
	msg_len = msg.stHead.usLen + sizeof(ST_ZJCT_MNG_HEADER);
	Zjct_AdptVoc_Socket_Send((uint8 *)&msg, msg_len);
#endif
	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}
	
	return DRV_OK;
}

static int32 Zjct_Ac491Voc_SendTo_Radio(uint8 *buf, uint32 len)
{
#if 0
	ST_ZJCT_MNG_MSG msg;
	int32 retval = 0;
	uint32 msg_len = 0;

	memset(&msg, 0, sizeof(msg));

	msg.stHead.ucType = ZJCT_MNG_TYPE_PCM128K;
	msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_DTJKH;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.usLen = ZJCT_VOC_PCM_20MS_LEN_320  + 3 ;  //��323�ֽڴ�����

	msg.ucData[0] = glocal_mem_update[id].data.dev_dscId & 0xff;

	msg.ucData[1] = glocal_mem_update[id].glocaldev_num;
	msg.ucData[2] = 0x00;							/*�����ֽ�*/

	retval = Pcm_Voice_Alaw2Linear(buf + RTP_HEADER_SIZE, \
				ZJCT_VOC_G711_20MS_LEN_160, adapter_voc_buf);

	if(ZJCT_VOC_PCM_20MS_LEN_320 != retval)
	{
		ERR("Pcm_Voice_Alaw2Linear error retval  = %d !\r\n", retval);
		return DRV_ERR;
	}

	memcpy(&msg.ucData[3], adapter_voc_buf, ZJCT_VOC_PCM_20MS_LEN_320);
	
	msg_len = msg.stHead.usLen + sizeof(ST_ZJCT_MNG_HEADER);
	Zjct_AdptVoc_Socket_Send((uint8 *)&msg, msg_len);
#endif
	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}
	
	return DRV_OK;
}

static int32  Zjct_Ac491Voc_SendTo_Adapter_Inc(uint8 *buf, uint32 len, uint8 id)
{

	uint8 sendto_flg = ZJCT_SENDTO_NULL;

	/* ���쳵ͨ������*/

	switch(glocal_mem_update[id].data.dev_mode)
	{
		case ZJCT_MODE_QHFS:
			/* ǿ������*/
			
			sendto_flg = ZJCT_SENDTO_ADAPTER;

			break;
		case ZJCT_MODE_QHJS:
			/* ǿ������*/
			
			/*��ֹ��������*/
			
			sendto_flg = ZJCT_SENDTO_NULL;
			break;
		case ZJCT_MODE_CTZH:
			/* ��ͨ���*/
			sendto_flg = ZJCT_SENDTO_GROUP;
			break;
		case ZJCT_MODE_DH:
			/* ����*/

			sendto_flg = ZJCT_SENDTO_ADAPTER;
			
			break;
		case ZJCT_MODE_CNGB:
			/* ���ڹ㲥*/

			sendto_flg = ZJCT_SENDTO_ADAPTER;
			
			break;	
		case ZJCT_MODE_KZDTFS:
			/* ���Ƶ�̨����*/
			if(RADIO_CTL_CONNECT == glocal_mem_update[id].radio_ctl_status)
			{

				sendto_flg = ZJCT_SENDTO_RADIO;
				
			}
			else if(RADIO_CTL_DISCONNECT == glocal_mem_update[id].radio_ctl_status)
			{
			
				/*��ֹ��������*/
				sendto_flg = ZJCT_SENDTO_NULL;
			}
			
			break;
		case ZJCT_MODE_KZDTJS:
			/* ���Ƶ�̨����*/
			
				/*��ֹ��������*/
				sendto_flg = ZJCT_SENDTO_NULL;
			break;
		case ZJCT_MODE_GTFS:
			/* ��������*/
			
			if(RADIO_CTL_CONNECT == glocal_mem_update[id].radio_gtctl_status)
			{
				sendto_flg = ZJCT_SENDTO_RADIO;	
			}
			else if(RADIO_CTL_DISCONNECT == glocal_mem_update[id].radio_gtctl_status)
			{
				/*��ֹ��������*/
				sendto_flg = ZJCT_SENDTO_NULL;
			}
			
			break;
		case ZJCT_MODE_GTJS:
			/* ��������*/


			break;
		case ZJCT_MODE_BHFS:
			/* ���Ž�������*/
			if(ZJCT_INC_CONNECT == glocal_mem_update[id].inc_connect)
			{
				sendto_flg = ZJCT_SENDTO_INC;
			}
			else
			{
				sendto_flg = ZJCT_SENDTO_NULL;
			}
			
			break;
		case ZJCT_MODE_BHJS:
			/* ���Ž�������*/
			if(ZJCT_INC_CONNECT == glocal_mem_update[id].inc_connect)
			{				
				sendto_flg = ZJCT_SENDTO_INC;
			}
			else
			{
				sendto_flg = ZJCT_SENDTO_NULL;
			}
			
			break;
		default:
			ERR(" Zjct_Ac491Voc_SendTo_Adapter_Inc mode error glocal_mem_update.data.dev_mode = 0x%x\r\n", \
					glocal_mem_update[id].data.dev_mode);
			break;		
	}

	if(ZJCT_SENDTO_ADAPTER == sendto_flg)
	{
		Zjct_Ac491Voc_SendTo_Adapter(buf, len);
	}
	else if(ZJCT_SENDTO_INC == sendto_flg)
	{		
		Zjct_Ac491Voc_SendTo_Inc(buf, len, 0);
	}
	else if(ZJCT_SENDTO_RADIO == sendto_flg)
	{
		Zjct_Ac491Voc_SendTo_Radio(buf, len);
	}
	else if(ZJCT_SENDTO_GROUP == sendto_flg)
	{
		Zjct_Ac491Voc_SendTo_Group(buf, len);
	}
	else
	{
		return DRV_OK;
	}

	return DRV_OK;
}



/*******************************************************************************
* Function: Zjct_Ac491Voc_To_MainBoard_Process
*
* Description: ��DSP  �Ļ������͵��������������û�
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
static int32  Zjct_Ac491Voc_To_MainBoard_Process(uint16 btimeslot, uint8 *buf, uint32 len)
{
	if ((buf == NULL)||(len == 0))//96-65����Ҫ��1�ģ���65��Ϊ������
	{
		ERR ("Ac491 rec voicedata is error:slot=%d\r\n", btimeslot);
		return DRV_ERR;
	} 

	/* 20ms����Ϊ160�ֽڣ���12�ֽ�RTPͷ*/
	if (RTP_RXBUFF_SIZE != len)
	{
		ERR ("Ac491 rec voicedata len error:size=%d\r\n", len);
		return DRV_ERR;
	}
	
	/*DSP  �������ݹ㲥���͵���ͨϵͳ*/
	Zjct_Ac491Voc_SendTo_Adapter_Inc(buf, len, 0);
	
	return DRV_OK;
}


/* */
int32 Zjct_Ac491Voc_To_MainBoard(uint16 btimeslot, uint8 *buf, uint32 len)
{
	/* */
	if(0 != btimeslot)
	{
		return DRV_OK;
	}

	if((NULL == buf)||(0 == len))
	{
		ERR("Zjct_Ac491Voc_To_MainBoard error buf = 0x%x\r\n", buf);
		return DRV_ERR;
	}
	
	if((FLG_OK == GET_SOCKTINIT_FLG()&&( PANEL_ONLINE == panel_status))||\
		(1 == g_dbg_panel))
	{
		Zjct_Ac491Voc_To_MainBoard_Process(btimeslot, buf, len);
		//Zjct_Ac491_Loopback_Test(btimeslot, p, len);
	}
	
	return DRV_OK;
}


static int32 Zjct_AdptVoc_Receive_From_Radio(uint8 *buf, uint32 len, uint8 id)
{
	uint8 alawbuf[ZJCT_VOC_G711_20MS_LEN_160] = {0};
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;
	uint8 sendto_ac491 = 0;
	uint32 templen = 0;
	uint16 index = 0;

	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}

	index = msg->ucData[1];
	if(index >= RADIO_NUM_MAX)
	{
		ERR("%s: dev_idx too big = %u\r\n", __func__, index);
		return DRV_ERR;
	}

	switch(glocal_mem_update[id].data.dev_mode)
	{
		case ZJCT_MODE_QHFS:
			/* ǿ������*/

			break;
		case ZJCT_MODE_QHJS:
			/* ǿ������*/

			break;
		case ZJCT_MODE_CTZH:
			/* ��ͨ���*/
			
			break;
		case ZJCT_MODE_DH:
			/* ����*/

			break;
		case ZJCT_MODE_CNGB:
			/* ���ڹ㲥*/
						
			break;	
		case ZJCT_MODE_KZDTFS:
			/* ���Ƶ�̨����*/
			
			break;
		case ZJCT_MODE_KZDTJS:
			/*	���Ƶ�̨����*/
			
			/*	�����Լ�������̨�Ļ�����*/
			if( glocal_mem_update[id].data.dev_dscId == (DTJKH_F100 | msg->ucData[1]))
			{
				sendto_ac491 = ZJCT_MODE_KZDTJS;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			
			break;
		case ZJCT_MODE_GTFS:
			/* ��������*/
			
			break;
		case ZJCT_MODE_GTJS:
			/* ��������*/
		#if 0
			if(ZJCT_DEV_ADDR_DTJKH == msg->stHead.ucSrcAddr)
			{
				sendto_ac491 = ZJCT_MODE_GTJS;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
		#else
			sendto_ac491 = ZJCT_MODE_NULL;
		#endif
		
			break;
		case ZJCT_MODE_BHFS:
			/* ���Ž�������*/
			
			break;
		case ZJCT_MODE_BHJS:
			/* ���Ž�������*/
			
			break;
		default:
			ERR("Zjct_AdptVoc_Receive_From_Radio mode error"
				"glocal_mem_update.data.dev_mode = 0x%x\r\n", \
					glocal_mem_update[id].data.dev_mode);
			break;	
	}


	{
		if(sendto_ac491)
		{
/*		
			templen = Pcm_Voice_Linear2Alaw(&msg->ucData[3], \
						ZJCT_VOC_G711_20MS_LEN_160 * 2, alawbuf);
*/
			if(CHAN_NOCFG == g_chan_auto_cfg)
			{
			
				if (index == 0x01)
				{
					//Zjct_Conference_Mainboard_To_Ac491(8, alawbuf, templen);
				}
				else if (index == 0x02)
				{
					Zjct_Conference_Mainboard_To_Ac491(10, alawbuf, templen);
				}
				else if (index == 0x03)
				{
					Zjct_Conference_Mainboard_To_Ac491(11, alawbuf, templen);
				}
				else if (index == 0x06)
				{
					//Zjct_Conference_Mainboard_To_Ac491(11, alawbuf, templen);
				}
				else
				{
					sendto_ac491 = ZJCT_MODE_NULL;	
				}
			}
			else
			{
				Zjct_Conference_Mainboard_To_Ac491(AC491_VOC_CHAN_FOR_RADIO, alawbuf, templen);
			}

			sendto_ac491 = ZJCT_MODE_NULL;	
		}
	}

	return DRV_OK;
}

static int32 Zjct_AdptVoc_Receive_From_Mem1(uint8 *buf, uint32 len)
{
	uint8 alawbuf[ZJCT_VOC_G711_20MS_LEN_160] = {0};
	uint32 templen = 0;
	int16 dev_type = 0, dev_idx = 0, index = 0;
	uint8 sendto_ac491 = 0;
	
	ST_ZJCT_MNG_MSG Msg,  *msg = NULL;
	memset(&Msg, 0, sizeof(Msg));
	memcpy(&Msg, buf, len);

	msg = &Msg;
	
	dev_type = CY1HH_TYPE_IDX;
	dev_idx = msg->ucData[1] - 1;			/* 0 ~ 31 */
	if(dev_idx >= ZJCT_DEV_INDX_MAX)
	{
		ERR("%s: dev_idx too big = %u\r\n", __func__, dev_idx);
		return DRV_ERR;
	}
	
	index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

	if(0 == gremote_mem_update[index].online)
	{
		return DRV_OK;
	}
		
	
	switch(glocal_mem_update[0].data.dev_mode)
	{
		case ZJCT_MODE_QHFS:
			/* ǿ������*/
			sendto_ac491 = ZJCT_MODE_NULL;
			break;
		case ZJCT_MODE_QHJS:
			/* ǿ������*/
			if(msg->ucData[1] == glocal_mem_update[0].force_call_srcId)
			{
				sendto_ac491 = ZJCT_MODE_QHJS;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			break;
		case ZJCT_MODE_CTZH:
			/* ��ͨ���*/

			if(ZJCT_CONNECT == gremote_mem_update[index].group_call_status)
			{
				sendto_ac491 = ZJCT_MODE_CTZH;
			}
			else if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			
			break;
		case ZJCT_MODE_DH:
			/* ����*/		
			/*���ڵ�����ȻҪ���չ㲥��*/
			if((glocal_mem_update[0].data.dev_dscId == (msg->ucData[1] |CY1HH_F400))\
				&& (1 == glocal_mem_update[0].connect))
			{	
				sendto_ac491 = ZJCT_MODE_DH;
			}
			else if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}

			break;
		case ZJCT_MODE_CNGB:
			/* ���ڹ㲥*/

			if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			
			break;	
		case ZJCT_MODE_KZDTFS:
			/* ���Ƶ�̨����*/

			/* ��ֹ���ջ�����*/
			sendto_ac491 = ZJCT_MODE_NULL;

			break;
		case ZJCT_MODE_KZDTJS:
			/* ���Ƶ�̨����*/
			
			/* ��������ͬһ����̨�ĳ�Ա������
			    ������ȻҪ���չ㲥��*/
			if(( glocal_mem_update[0].data.dev_dscId == gremote_mem_save[index].data.dev_dscId)\
			&&(ZJCT_MODE_KZDTFS == gremote_mem_save[index].data.dev_mode))
			{
				sendto_ac491 = ZJCT_MODE_KZDTJS;
			}
			else if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			
			break;
		case ZJCT_MODE_GTFS:
			/* ��������*/

			/* ��ֹ���ջ�����*/
			sendto_ac491 = ZJCT_MODE_NULL;
			
			break;
		case ZJCT_MODE_GTJS:
			/* ��������*/
			if(( glocal_mem_update[0].data.dev_dscId == gremote_mem_save[index].data.dev_dscId)\
				&&(ZJCT_MODE_KZDTFS == gremote_mem_save[index].data.dev_mode))
			{
				sendto_ac491 = ZJCT_MODE_GTJS;
			}
			else if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				/* �������д��ڳ�ͨģʽ�Ļ�����*/
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			break;
		case ZJCT_MODE_BHFS:
			/* ���Ž�������*/
			/*���ڵ�����ȻҪ���չ㲥��*/
			if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
				//sendto_ac491 = ZJCT_MODE_NULL;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			break;
		case ZJCT_MODE_BHJS:
			/* ���Ž�������*/
			
			/*���ڵ�����ȻҪ���չ㲥��*/
			if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
				//sendto_ac491 = ZJCT_MODE_NULL;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			break;
		default:
			ERR("%s mode error"
				"glocal_mem_update.data.dev_mode = 0x%x\r\n", \
					__func__, glocal_mem_update[0].data.dev_mode);
			break;		
	}
	

	if(sendto_ac491)
	{
		if(CHAN_NOCFG == g_chan_auto_cfg)	
		{
/*		
			templen = Pcm_Voice_Linear2Alaw(&msg->ucData[3], \
				ZJCT_VOC_PCM_20MS_LEN_320, alawbuf);
*/					
			if ((msg->ucData[1] == 0x05)&&(msg->ucData[1] < AC491_MAX_CHAN_ALL))
			{
				Zjct_Conference_Mainboard_To_Ac491(glocal_mem_update[0].glocaldev_num, alawbuf, templen);
			}
			else if ((msg->ucData[1] == 0x03)&&(msg->ucData[1] < AC491_MAX_CHAN_ALL))
			{
				Zjct_Conference_Mainboard_To_Ac491(9, alawbuf, templen);
			}
			else if ((msg->ucData[1] != 0x05)&&(msg->ucData[1] < AC491_MAX_CHAN_ALL))
			{
				Zjct_Conference_Mainboard_To_Ac491(msg->ucData[1], alawbuf, templen);
			}
			else
			{
				ERR("Zjct_AdptVoc_RxThread AC491 channel error msg->ucData[1]"
					"= %d\r\n", msg->ucData[1] );
				sendto_ac491 = ZJCT_MODE_NULL;	
				
				return DRV_ERR;
			}
		}
		else
		{
			if(0xFF !=  gremote_mem_update[index].mem_chan_id)
			{
/*
				templen = Pcm_Voice_Linear2Alaw(&msg->ucData[3], \
					ZJCT_VOC_PCM_20MS_LEN_320, alawbuf);
*/					
				Zjct_Conference_Mainboard_To_Ac491(gremote_mem_update[index].mem_chan_id, alawbuf, templen);
			}
			else
			{
				memcpy(&gremote_mem_update[index].mem_voc[gremote_mem_update[index].mem_save][0],\
						&msg->ucData[3], ZJCT_VOC_PCM_20MS_LEN_320);	
				gremote_mem_update[index].mem_save = (gremote_mem_update[index].mem_save + 1) % VOICE_TIME_1min;	
			}
		}
		
		sendto_ac491 = ZJCT_MODE_NULL;
	}

	return DRV_OK;
}

static int32 Zjct_AdptVoc_Receive_From_Mem2(uint8 *buf, uint32 len)
{
	uint8 alawbuf[ZJCT_VOC_G711_20MS_LEN_160] = {0};
	uint32 templen = 0;
	int16 dev_type = 0, dev_idx = 0, index = 0;
	uint8 sendto_ac491 = 0;
	
	ST_ZJCT_MNG_MSG Msg,  *msg = NULL;
	memset(&Msg, 0, sizeof(Msg));
	memcpy(&Msg, buf, len);

	msg = &Msg;
	
	dev_type = CY2HH_TYPE_IDX;
	dev_idx = msg->ucData[1] - 1;			/* 0 ~ 31 */
	if(dev_idx >= ZJCT_DEV_INDX_MAX)
	{
		ERR("%s: dev_idx too big = %u\r\n", __func__, dev_idx);
		return DRV_ERR;
	}
	
	index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

	if(0 == gremote_mem_update[index].online)
	{
		return DRV_OK;
	}
		
	
	switch(glocal_mem_update[0].data.dev_mode)
	{
		case ZJCT_MODE_QHFS:
			/* ǿ������*/
			sendto_ac491 = ZJCT_MODE_NULL;
			break;
		case ZJCT_MODE_QHJS:
			/* ǿ������*/
			if(msg->ucData[1] == glocal_mem_update[0].force_call_srcId)
			{
				sendto_ac491 = ZJCT_MODE_QHJS;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			break;
		case ZJCT_MODE_CTZH:
			/* ��ͨ���*/
			 if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			
			break;
		case ZJCT_MODE_DH:
			/* ����*/
			
			/*���ڵ�����ȻҪ���չ㲥��*/
			if((glocal_mem_update[0].data.dev_dscId == (msg->ucData[1] |CY1HH_F400))\
				&& (1 == glocal_mem_update[0].connect))
			{	
				sendto_ac491 = ZJCT_MODE_DH;
			}
			else if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}

			break;
		case ZJCT_MODE_CNGB:
			/* ���ڹ㲥*/
			if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			
			break;	
		case ZJCT_MODE_KZDTFS:
			/* ���Ƶ�̨����*/

			/* ��ֹ���ջ�����*/
			sendto_ac491 = ZJCT_MODE_NULL;

			break;
		case ZJCT_MODE_KZDTJS:
			/* ���Ƶ�̨����*/
			
			/* ��������ͬһ����̨�ĳ�Ա������
			    ������ȻҪ���չ㲥��*/
			if(( glocal_mem_update[0].data.dev_dscId == gremote_mem_save[index].data.dev_dscId)\
			&&(ZJCT_MODE_KZDTFS == gremote_mem_save[index].data.dev_mode))
			{
				sendto_ac491 = ZJCT_MODE_KZDTJS;
			}
			else if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			
			break;
		case ZJCT_MODE_GTFS:
			/* ��������*/

			/* ��ֹ���ջ�����*/
			sendto_ac491 = ZJCT_MODE_NULL;
			
			break;
		case ZJCT_MODE_GTJS:
			/* ��������*/
			if(( glocal_mem_update[0].data.dev_dscId == gremote_mem_save[index].data.dev_dscId)\
				&&(ZJCT_MODE_KZDTFS == gremote_mem_save[index].data.dev_mode))
			{
				sendto_ac491 = ZJCT_MODE_GTJS;
			}
			else if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				/* �������д��ڳ�ͨģʽ�Ļ�����*/
				sendto_ac491 = ZJCT_MODE_CNGB;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			break;
		case ZJCT_MODE_BHFS:
			/* ���Ž�������*/
			/*���ڵ�����ȻҪ���չ㲥��*/
			if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
				//sendto_ac491 = ZJCT_MODE_NULL;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			break;
		case ZJCT_MODE_BHJS:
			/* ���Ž�������*/
			
			/*���ڵ�����ȻҪ���չ㲥��*/
			if(ZJCT_MODE_CNGB == gremote_mem_save[index].data.dev_mode)
			{
				sendto_ac491 = ZJCT_MODE_CNGB;
				//sendto_ac491 = ZJCT_MODE_NULL;
			}
			else
			{
				sendto_ac491 = ZJCT_MODE_NULL;
			}
			break;
		default:
			ERR("%s mode error"
				"glocal_mem_update.data.dev_mode = 0x%x\r\n", \
					__func__, glocal_mem_update[0].data.dev_mode);
			break;		
	}
	

	{
		if(sendto_ac491)
		{
/*		
			templen = Pcm_Voice_Linear2Alaw(&msg->ucData[3], \
						ZJCT_VOC_PCM_20MS_LEN_320, alawbuf);
*/
			if(CHAN_NOCFG == g_chan_auto_cfg)	
			{
				if ((msg->ucData[1] == 0x05)&&(msg->ucData[1] < AC491_MAX_CHAN_ALL))
				{
					Zjct_Conference_Mainboard_To_Ac491(glocal_mem_update[0].glocaldev_num, alawbuf, templen);
				}
				else if ((msg->ucData[1] == 0x03)&&(msg->ucData[1] < AC491_MAX_CHAN_ALL))
				{
					Zjct_Conference_Mainboard_To_Ac491(9, alawbuf, templen);
				}
				else if ((msg->ucData[1] != 0x05)&&(msg->ucData[1] < AC491_MAX_CHAN_ALL))
				{
					Zjct_Conference_Mainboard_To_Ac491(msg->ucData[1], alawbuf, templen);
				}
				else
				{
					ERR("Zjct_AdptVoc_RxThread AC491 channel error msg->ucData[1]"
						"= %d\r\n", msg->ucData[1] );
					sendto_ac491 = ZJCT_MODE_NULL;	
					
					return DRV_ERR;
				}
			}
			else
			{
				if(0xFF !=  gremote_mem_update[index].mem_chan_id)
				{
					Zjct_Conference_Mainboard_To_Ac491(gremote_mem_update[index].mem_chan_id, alawbuf, templen);
				}
				else
				{
					memcpy(&gremote_mem_update[index].mem_voc[gremote_mem_update[index].mem_save][0],\
							&msg->ucData[3], ZJCT_VOC_PCM_20MS_LEN_320);	
					gremote_mem_update[index].mem_save = (gremote_mem_update[index].mem_save + 1) % VOICE_TIME_1min;	
				}
			}
			
			sendto_ac491 = ZJCT_MODE_NULL;
		}
	}

	return DRV_OK;
}

static int32 Zjct_AdptVoc_Msg_Process(uint8 *buf, uint32 len)
{
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;
				
	switch(msg->stHead.ucSrcAddr)  
	{
		case ZJCT_DEV_ADDR_CY1H:
			if(DRV_OK != Zjct_AdptVoc_Receive_From_Mem1(buf, len))
			{
				ERR("Zjct_AdptVoc_Receive_From_Mem1\r\n");
				return DRV_ERR;
			}		
			break;
		case ZJCT_DEV_ADDR_CY2H:	
			if(DRV_OK != Zjct_AdptVoc_Receive_From_Mem2(buf, len))
			{
				ERR("Zjct_AdptVoc_Receive_From_Mem2\r\n");
				return DRV_ERR;
			}
			
			break;			
		case ZJCT_DEV_ADDR_DTJKH:			
			if(DRV_OK != Zjct_AdptVoc_Receive_From_Radio(buf, len, 0))
			{
				ERR("Zjct_AdptVoc_Receive_From_Radio\r\n");
				return DRV_ERR;
			}
			break;
		case ZJCT_DEV_ADDR_YHKZH:

			break;
		default:
			ERR("Zjct_AdptVoc_Msg_Process param error"
				"msg->stHead.ucSrcAddr = 0x%x\r\n", msg->stHead.ucSrcAddr);
			return DRV_ERR;
			break;
	}

	return DRV_OK;
}

static int32 Zjct_Conference_Mainboard_To_Ac491(uint32 channel, uint8 *buf, uint32 len)
{	
	uint8 sendbuf[RTP_TXBUFF_SIZE];
	struct TRTPHeader *pRTPHeader;	
	
	static uint32 seq[36] = {0};
	static uint32 s_timeslot[36] = {0};
		
	if (ZJCT_VOC_G711_20MS_LEN_160 != len)
	{
		ERR("20ms G711 voice len (%d)!=160 \n", len);
		return DRV_ERR;
	}
	
	pRTPHeader = (struct TRTPHeader *)&sendbuf[0]; 
	memcpy(&sendbuf[sizeof(struct TRTPHeader)],  buf , ZJCT_VOC_G711_20MS_LEN_160);
	
	pRTPHeader->RTPVer = 2;
	pRTPHeader->RTPPadding = 0;
	pRTPHeader->RTPExt = 0;
	pRTPHeader->RTPCSRCCnt = 0;
	pRTPHeader->RTPMarker = 0;
	pRTPHeader->RTPPT = 0x08; //lishibing ,G711ʱ������0x12,PCM����Ϊ0x08
	
	pRTPHeader->RTPSSRC = 0x11111111; //lishibing ,G711ʱ������0x23482900;PCM����Ϊ0x11111111.
	pRTPHeader->RTPSeqNum = seq[channel];	
	seq[channel]++;
	pRTPHeader->RTPTimeStamp = s_timeslot[channel];
	s_timeslot[channel] += 160;
	pRTPHeader->RTPSeqNum = ntohs(pRTPHeader->RTPSeqNum);		
	pRTPHeader->RTPSSRC = ntohl(pRTPHeader->RTPSSRC);

	if (0 != ac491SendData(channel/NUMBER_OF_CHANNELS, channel%NUMBER_OF_CHANNELS, sendbuf,  RTP_TXBUFF_SIZE,  PACK_TYPE_RTP))
	{
		ERR("%s: ac491SendData  failed\r\n", __func__);
	}

//	usleep(5000);

	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_IncCall_SendOpenUserMsg
*
* Description: ��II���Լ�ɹ�����ʱ��ͨ���������Ϳ�ͨ��Ϣ��
*                   ��ͨ���������յ���ͨ��Ϣ�󣬸�II�ͷ���ע����Ϣ��
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
int32 Zjct_IncCall_SendOpenUserMsg(void)
{
	ST_CALL_SEM_MSG msg;
	uint32  j = 0;
	uint32  msg_len = 0;

	LOG("Zjct_IncCall_SendOpenUserMsg \r\n");

	memset(&msg, 0, sizeof(msg));
	
	/* ��仰������Э��ͷ*/
	msg.Head.ucProType = ZJCT_PRO_TYPE;
	msg.Head.ucProVer = ZJCT_PRO_VER;
	msg.Head.ucSemType = ZJCT_INC_CONFIG;
	
	j = 0;

	glocal_incmem.mem_num = glocal_mem_update[0].glocaldev_num;
	msg.MsgData.subData[j++] = 1;
	msg.MsgData.subData[j++] = glocal_mem_update[0].glocaldev_num;

	msg.MsgData.subType = ZJCT_CALL_CFG_USEROPEN_REQ;
	msg.MsgData.subLen = j ;	
	msg.Head.usLen = msg.MsgData.subLen + ZJCT_SUBDATA_HEADLEN;
			
	msg_len = msg.Head.usLen+ZJCT_CALL_SEM_MSG_HEADER_LEN;
	
	//Zjct_IncCall_Socket_Send((uint8 *)&msg, msg_len);
	
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_Call_Register_UserPhoneNumber
*
* Description: ��ͨ����ע���û����롣
	��ͨ��ͨ����������ע��������Ϣ����ʱ��Ϣ�����Ϊ0x04��
	�����豸�������ע��Ӧ����Ϣ������Ӧ����ʱ��Ϣ�����Ϊ0x05��
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
int32 Zjct_IncCall_Register_UserPhoneNumber(uint8 *buf, uint32 len)
{
	ST_CALL_SEM_MSG msg;
	int32 i = 0,  j = 0;
	uint32 msg_len = 0;
	
	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}
	
	/* ��仰������Э��ͷ*/
	msg.Head.ucProType = ZJCT_PRO_TYPE;
	msg.Head.ucProVer = ZJCT_PRO_VER;
	msg.Head.ucSemType = ZJCT_INC_CONFIG;
	msg.Head.usLen = 0x00;//��Ϣ�ܳ���len��ʱ��0
	
	glocal_incmem.mem_num = i + 1;
	msg.MsgData.subData[++j] = glocal_incmem.mem_num;
	msg.MsgData.subData[++j] = glocal_incmem.pnum_len;
	memcpy(&msg.MsgData.subData[++j], glocal_incmem.user_num, 10);
	i += 9;
	msg.MsgData.subData[++j] = glocal_incmem.encode_type;
			
	msg.MsgData.subType = ZJCT_CALL_CFG_USERPHONENUM_REGISTER_REQ;
	msg.MsgData.subLen = j + 1;	//����len��ʱ��0��ָ����λ
	
	msg.Head.usLen = msg.MsgData.subLen + ZJCT_SUBDATA_HEADLEN;
	
	msg_len = msg.Head.usLen + ZJCT_CALL_SEM_MSG_HEADER_LEN;
	//Zjct_IncCall_Socket_Send((uint8 *)&msg, msg_len);
	
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdtMng_SendRegisterNetwork_Req_Ack
*
* Description:  ������������Ӧ��

*
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
static int32 Zjct_AdptMng_SendRegisterNetwork_Req_Ack( uint8 * buf, uint32 len)
{
    if((NULL == buf)||(0 == len))
    {
        return DRV_ERR;
    }

	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdtMng_SendRegisterNetwork_Req
*
* Description: DHCP��ȡ��ַ�ɹ��󣬵�̨�ӿںС�
	��̨�ӿڵ�Ԫ����Ա����������"��������"��Ϣ��
	�����Ŀ��ƺ����ص�Ԫ������������
	����"����Ӧ��"��Ϣ����Ӧ��
	"����Ӧ��"��Ϣ��Я��һ��PDUΪʶ��PDU��
	��̨�ӿںС���̨�ӿڵ�Ԫ��
	��Ա�������ʱδ�յ�"����Ӧ��"��Ϣ��
	��ʱ���������"��������"��Ϣ��
	��ʱʱ���ݶ�Ϊ3s

*
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
int32 Zjct_AdptMng_SendRegisterNetwork_Req(uint8 id)
{
	ST_ZJCT_MNG_MSG msg;
	uint32 msg_len = 0;
	
	LOG("%s\r\n", __func__);

	memset(&msg, 0, sizeof(msg));

	/* �����Ϣͷ*/
	msg.stHead.ucType = ZJCT_MNG_TYPE_REGREQ;
	msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_ZHZXH;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.usLen = 0x09; 

	/* �����Ϣ����*/
	msg.ucData[0] = 0x00; //des terminal addr
	msg.ucData[1] = glocal_mem_update[id].glocaldev_num;//src terminal addr
	msg.ucData[2] = 0x00;//inf num
	msg.ucData[3] = 0x00;
	msg.ucData[4] = 0x00;  //port 
	
	msg.ucData[5] = 0x00; //length
	msg.ucData[6] = 0x02;

	msg.ucData[7] = 0x00; //type
	msg.ucData[8] = 0x00;
	
	/* ������Ϣ*/
	msg_len = msg.stHead.usLen + ZJCT_MNG_MSG_HEADER_LEN;

	Zjct_Sendto_Adapter_Socket((uint8 *)&msg , msg_len, id);
	
	return DRV_OK;		
}

/*******************************************************************************
* Function: Zjct_AdtMng_Param_Set_Ack
*
* Description:  ��ͨ���������Ͳ����趨��Ӧ
	
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
static int32 Zjct_AdptMng_SendTo_Param_Set_Ack(uint8 *buf, uint16 Ack_Len)
{	
	ST_ZJCT_MNG_MSG *temp_msg = (ST_ZJCT_MNG_MSG *)buf;
	ST_ZJCT_MNG_MSG msg;
	uint32 msg_len = 0;

	if((NULL == buf)||(0 == Ack_Len))
	{
        return DRV_ERR;
	}

	memset(&msg, 0, sizeof(msg));
	
	/*�����Ϣͷ*/
	msg.stHead.ucType = ZJCT_MNG_TYPE_CONFIG_ACK;
	msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;

	/* �����Ϣ����*/
	msg.ucData[0] = temp_msg->ucData[1];
	msg.ucData[1] = glocal_mem_update[0].glocaldev_num;
	msg.ucData[2] = temp_msg->ucData[2];
	msg.ucData[3] = 0x00;		
	
	msg.stHead.usLen = 4;
	
	msg_len = msg.stHead.usLen + ZJCT_MNG_MSG_HEADER_LEN;
	
	/* ������Ϣ*/
	//Zjct_AdptMng_Socket_Send((uint8 *)&msg , msg_len);

	return DRV_OK;
}


/*******************************************************************************
* Function: Zjct_AdtMng_Param_Set
*
* Description:  ��������֡����ָ����/ս����
���Ŀ��ƺ�ͨ����̫���ڶԳ�ͨ�豸ת�����������ݣ�
��֡�ṹ�����ݲ��ֽṹ
	
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
static int32 Zjct_AdptMng_Receive_Param_Set_Ack(uint8 *buf, uint32 len)
{

	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;

	uint8 Ack_data[12] = {0};  // spec �������Ϊ8Byte

	int16 dev_type = 0, dev_idx = 0, index =0;

	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}

	memset(Ack_data, 0, sizeof(Ack_data));
	
	switch(msg->ucData[2])
	{			
		case ZJCT_GROUPCALLMEM_START:
			
			dev_type = CY1HH_TYPE_IDX;
			dev_idx = msg->ucData[1] - 1;
			if(dev_idx >= ZJCT_DEV_INDX_MAX)
			{
				ERR("%s: dev_idx too big = %u\r\n", __func__, dev_idx);
				break;
			}
			
			index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

			if( glocal_mem_update[0].glocaldev_num == msg->ucData[0])
			{
				if((ZJCT_SET_OK == msg->ucData[3])
					&&(ZJCT_DISCONNECT == gremote_mem_update[index].group_call_status))
				{
					gremote_mem_update[index].group_call_status = ZJCT_CONNECT;

					g_groudcall_cnt++;

					if(g_groudcall_cnt >= (groupcall_mem[0] - 1))
					{
						gauto_send_cfg_flg = 0;	
					}

					DBG("send Member %d add the group\r\n", msg->ucData[1]);
				}
			}

			break;
		case ZJCT_GROUPCALLMEM_END:
		
			break;
		default:
			ERR("%s failed msg->ucData[2] = 0x%x\r\n",\
					__func__, msg->ucData[2]);
			return DRV_ERR;
			break;
	}

	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdtMng_Param_Set
*
* Description:  ��������֡����ָ����/ս����
���Ŀ��ƺ�ͨ����̫���ڶԳ�ͨ�豸ת�����������ݣ�
��֡�ṹ�����ݲ��ֽṹ
	
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
static int32 Zjct_AdptMng_Sendto_PC_GroupStart(void)
{
	ST_ZJCT_MNG_MSG msg;
	memset(&msg, 0, sizeof(msg));

	msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
	msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;

	msg.ucData[3] = ZJCT_MODE_CTZH;	
	msg.ucData[DANHU_MSG_BYTE] = GROUPCALLIN;
	msg.ucData[DANHU_MSG_BYTE + 1] = groupcall_mem[0];
	memcpy(&msg.ucData[DANHU_MSG_BYTE + 2], \
			&groupcall_mem[1], groupcall_mem[0]);

	Zjct_SeatMng_Socket_Send((uint8 *)&msg, ZJCT_STATUS_MSG_LEN);

	return DRV_OK;
}

static int32 Zjct_AdptMng_Sendto_PC_GroupEnd(void)
{
	ST_ZJCT_MNG_MSG msg;
	memset(&msg, 0, sizeof(msg));

	msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
	msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
	
	msg.ucData[DANHU_MSG_BYTE] = CANCEL_GROUPCALLIN;
	msg.ucData[3] = ZJCT_MODE_CTZH;

	Zjct_SeatMng_Socket_Send((uint8 *)&msg, ZJCT_STATUS_MSG_LEN);
	
	return DRV_OK;
}

static int32 Zjct_AdptMng_Receive_Param_Set(uint8 *buf, uint32 len)
{

	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;
	uint16 Ack_Len = 0 , param_len = 0;
	uint8 Ack_data[12] = {0};  // ?? spec �������Ϊ8Byte
	
	uint16 i = 0, j = 0;
	uint8 send_flg = 0;

	int16 dev_type = 0, dev_idx = 0, index =0;

	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}	

	memset(Ack_data, 0, sizeof(Ack_data));

	switch(msg->ucData[2])
	{
		case ZJCT_SEND_MSG_CONTROL:

			break;
		case ZJCT_LOWER_MEM_LISTEN_RANGE:

			break;
		case ZJCT_SUBNET_IP_ADDR:

			break;		
		case ZJCT_GROUPCALLMEM_START:

			param_len = msg->stHead.usLen - 3;

			for (j = 0; j < param_len; j++)
			{
				if(( glocal_mem_update[0].glocaldev_num == msg->ucData[j + 3])\
					&&(ZJCT_MODE_DH != glocal_mem_update[0].data.dev_mode)\
					&&(ZJCT_MODE_QHFS != glocal_mem_update[0].data.dev_mode)\
					&&(ZJCT_MODE_QHJS != glocal_mem_update[0].data.dev_mode)\
					&&(ZJCT_MODE_CTZH != glocal_mem_update[0].data.dev_mode)\
					&&(ZJCT_MODE_GTFS != glocal_mem_update[0].data.dev_mode)\
					&&(ZJCT_MODE_BHFS != glocal_mem_update[0].data.dev_mode)\
					&&(ZJCT_MODE_BHJS != glocal_mem_update[0].data.dev_mode)\
					&&(ZJCT_MODE_DTFS != glocal_mem_update[0].data.dev_mode)\
					&&(0 == glocal_mem_update[0].callIn)\
					&&(0 == glocal_mem_update[0].inc_callIn))					
				{	

					Zjct_Local_Mem_Data_Push((uint8 *)&glocal_mem_update[0].data,\
							ZJCT_STATUS_MSG_DATA_LEN);

					glocal_mem_update[0].data.dev_mode = ZJCT_MODE_CTZH;
					glocal_mem_update[0].data.dev_dscId = CY1HH_F400 | msg->ucData[1];

					groupcall_mem[0] = param_len;
					memcpy(&groupcall_mem[1], &msg->ucData[3], groupcall_mem[0]);

					dev_type = CY1HH_TYPE_IDX;

					for (i = 0; i < groupcall_mem[0]; i++)
					{
						dev_idx = groupcall_mem[i + 1] - 1;
						if(dev_idx >= ZJCT_DEV_INDX_MAX)
						{
							ERR("%s: dev_idx too big = %u\r\n", __func__, dev_idx);
							break;
						}
						
						index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

						gremote_mem_update[index].group_call_status = ZJCT_CONNECT;

						DBG("receive Member %d add the group\r\n", groupcall_mem[i + 1]);
					}
					
					DBG("receive a grounp call start\r\n");

					glocal_mem_update[0].group_call_id = msg->ucData[1];

					Zjct_AdptMng_Sendto_PC_GroupStart();

					send_flg = 1;
					
					break;
				}
				else
				{
					LOG("mode %d, callin %d, inc_callin %d\r\n", glocal_mem_update[0].data.dev_mode,\
						glocal_mem_update[0].callIn, glocal_mem_update[0].inc_callIn);
				}
			}
			
			break;
		case ZJCT_GROUPCALLMEM_END:
			param_len = msg->stHead.usLen - 3;

			for (j = 0; j < param_len; j++)
			{
				if ((glocal_mem_update[0].group_call_id == msg->ucData[1])&&(glocal_mem_update[0].glocaldev_num == msg->ucData[j + 3])
					&&(ZJCT_MODE_CTZH == glocal_mem_update[0].data.dev_mode))
				{
					Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update[0].data,\
							ZJCT_STATUS_MSG_DATA_LEN);

					dev_type = CY1HH_TYPE_IDX;
					for (i = 0; i < groupcall_mem[0]; i++)
					{
						dev_idx = groupcall_mem[i + 1] - 1;
						if(dev_idx >= ZJCT_DEV_INDX_MAX)
						{
							ERR("%s: dev_idx too big = %u\r\n", __func__, dev_idx);
							break;
						}
						
						index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

						gremote_mem_update[index].group_call_status = ZJCT_DISCONNECT;
						DBG("receive Member %d leave the group\r\n", groupcall_mem[i + 1]);
						
					}
					groupcall_mem[0] = 0x00;
						
					DBG("receive a grounp call end\r\n");
					
					Zjct_AdptMng_Sendto_PC_GroupEnd();

					send_flg = 1;
					
					break;
				}
			}
		
			break;
		case ZJCT_LOWER_M_VALUE:

			break;
		case ZJCT_LOWER_MEM_SENDTO:

			break;
		case ZJCT_HIGH_PERMISSION:

			break;
		case ZJCT_LOWER_PARAM_INQUIRE:

			break;
		default:
			ERR("%s failed msg->ucData[2] = 0x%x\r\n",\
					__func__, msg->ucData[2]);
			//return DRV_ERR;
			break;
	}

	if(1 == send_flg)
	{
		Zjct_AdptMng_SendTo_Param_Set_Ack(buf , Ack_Len);
	}
	
	return DRV_OK;
}
/*******************************************************************************
* Function: Zjct_AdtMng_SendParamConfig_Ack
*
* Description: ����������������������ò���Ӧ��
	
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
static int32 Zjct_AdptMng_SendParamConfig_Ack(uint8 *buf, uint32 len)
{

	ST_ZJCT_MNG_MSG msg;
	uint32 msg_len = 0;

	DBG("Zjct_AdtMng_SendParamConfig_Ack \r\n");
	/* �����Ϣͷ*/
	msg.stHead.ucType = ZJCT_MNG_TYPE_PARAMCFGACK;
	msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_ZHZXH;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.usLen = 0x09; //???

	/* �����Ϣ����*/
	msg.ucData[0] = 0x01; //des terminal addr
//	msg.ucData[1] = 0x08;//src terminal addr
	msg.ucData[1] = glocal_mem_update[0].glocaldev_num;
	msg.ucData[2] = buf[7];//
	msg.ucData[3] = buf[8];
	msg.ucData[4] = 0x00;  //port 

	msg.ucData[5] = 0x00; //length
	msg.ucData[6] = 0x02;

	msg.ucData[7] = 0x00; //type
	msg.ucData[8] = 0x00;

	/* ������Ϣ*/
	msg_len = msg.stHead.usLen + ZJCT_MNG_MSG_HEADER_LEN;
	//Zjct_AdptMng_Socket_Send((uint8 *)&msg, msg_len);
	
	return DRV_OK;
}

/*******************************************************************************
*******************************************************************************
* Function: Zjct_AdtMng_SendParamConfig_Proc
*
* Description: ��������������������ò���
	
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
static int32 Zjct_AdptMng_SendParamConfig_Proc(uint8 *buf, uint32 len, uint8 id)
{
	//uint32 pdu_len = 0;
	uint8 *temp_buf = NULL;
	uint8 temp_cnt = 0;
	uint8 i = 0;
	
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;
	temp_buf = buf + 10;
	
	if(msg->stHead.usLen > 0x09)
	{
		while((msg->stHead.usLen) > (temp_buf - buf - 5))
		{
			switch(*((short *)(temp_buf + 2)))
			{
				case 0x04: 			/*Ip addr and netmask */
					temp_buf += 5;
					//memcpy(&glocal_mem_update.Ipaddr, temp_buf, 5);
					temp_buf += 5;
					DBG("glocal_mem_update.ip = 0x%x\r\n", glocal_mem_update[id].Ipaddr);
					DBG("glocal_mem_update.Netmask = 0x%x\r\n", glocal_mem_update[id].Netmask);
					//pdu_len += *(short *)(msg->ucData[5]);
					break;
				case 0x05:			/*user name and password */	

					temp_buf += 6;
					temp_cnt = strlen((char *)temp_buf);
					
					if(temp_cnt >= 10)
					{
	
						ERR("user_num length too long error temp_cnt = %d\r\n", temp_cnt);
						return DRV_ERR;
					}
					
					/*update the status frame data*/	
					glocal_mem_update[id].data.user_num[0] = 0x01;
					glocal_mem_update[id].data.user_num[1] = temp_cnt;
					
					//memcpy(&glocal_mem_update.data.user_num[2], temp_buf, temp_cnt);
					for(i = 0; i < temp_cnt; i++)
					{
						glocal_mem_update[id].data.user_num[2 + i] = *(temp_buf + i) - 0x30;
					}
					glocal_mem_update[id].data.user_num[temp_cnt + 2] = 0;
								
					DBG("glocal_mem_update.name = %s, temp_cnt = %d\r\n", \
							temp_buf, temp_cnt);
					
					temp_buf += (temp_cnt + 1);
					temp_cnt = strlen((char *)temp_buf);
					
					if(temp_cnt >= 10 )
					{
						ERR("password pdu length too long error temp_cnt = %d\r\n", temp_cnt);
						return DRV_ERR;
					}
					
					memcpy(glocal_mem_update[id].user_password, temp_buf,temp_cnt);
					glocal_mem_update[id].user_password[temp_cnt] = 0;
/*
					DBG("glocal_mem_update.user_password = %s\r\n", \
							glocal_mem_update.user_password);
*/									
					temp_buf += (temp_cnt + 1);
					
					//pdu_len += *(short *)(msg->ucData[5]);
	
					break;
				default:
					ERR("the config data from network PDU type error!\r\n");
					return DRV_ERR;
					break;
			}
			//DBG("temp_buf - buf - 5 = %d\r\n", temp_buf - buf - 5);
			
		}

	}
	else
	{
		ERR("the config data from network length error!\r\n");
		return DRV_ERR;
	}
	
	Zjct_AdptMng_SendParamConfig_Ack(buf, len);
	
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdtMng_SendParamConfig_Req
*
* Description:
	Ϊ��֤����������һ���ԣ��ڻ���������֮��
	��̨�ӿںС���̨�ӿڰ塢�û�����Ҫ������
	���Ŀ��ƺ����ص�Ԫ����"��������"��Ϣ��
	���ص�Ԫ���յ�"��������"��Ϣ�󽫷���
	�·� "��������"��Ϣ��"��������"��Ϣ�����ж�����
	���һ���·��������EOF��־��
	������ȡ��ͨ�Ź��̲���"����-��Ӧ"���ƣ�
	��̨�ӿںС���̨�ӿڰ塢�û����ڷ���"��������
	"��ʱ��"��������"��Ӧ������ط���
	ͬ�������ڷ���"��������"��ʱ��"����Ӧ��
	"��ӦҲҪ�����ط������ʹ���Ϊ1��
	��Ӧʱ��ȱʡΪ3s��
*
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
int32 Zjct_AdptMng_SendParamConfig_Req(uint8 id)
{
	ST_ZJCT_MNG_MSG msg;
	uint32 msg_len = 0;

	LOG("%s \r\n", __func__);

	/* �����Ϣͷ*/
	msg.stHead.ucType = ZJCT_MNG_TYPE_PARAMREQ;
	msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_ZHZXH;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.usLen = 0x09; //???

	/* �����Ϣ����*/
	msg.ucData[0] = 0x00; //des terminal addr
	msg.ucData[1] = glocal_mem_update[id].glocaldev_num;//src terminal addr
	msg.ucData[2] = 0x00;//inf num
	msg.ucData[3] = 0x00;
	msg.ucData[4] = 0x00;  //port 

	msg.ucData[5] = 0x00; //length
	msg.ucData[6] = 0x02;

	msg.ucData[7] = 0x00; //type
	msg.ucData[8] = 0x00;

	/* ������Ϣ*/

	msg_len = msg.stHead.usLen + ZJCT_MNG_MSG_HEADER_LEN;
	//Zjct_AdptMng_Socket_Send((uint8 *)&msg , msg_len);

	Zjct_Sendto_Adapter_Socket((uint8 *)&msg , msg_len, id);

	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdtMng_AutoSendStatusMsg
*
* Description:  ����ͨ�������������͹�����״̬��ѯ��Ϣ.
			 ���¼���Ŀ��ƺе�IP
*
* Inputs: buff -��Ϣ�г�ȥ����ͷ��������
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
int32 Zjct_AdptMng_AutoSendStatusMsg(uint8 id)
{
	ST_ZJCT_MNG_MSG msg ;
	uint32 msg_len = 0;

	LOG("%s start \r\n", __func__);
	
	memset(&msg, 0, sizeof(msg));

	if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
	{
		//return DRV_OK;
	}
	
	/* �����Ϣͷ*/
	msg.stHead.ucType = ZJCT_MNG_TYPE_DEV_STATUS_INFO;
	msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN; 
	
	/* �����Ϣ����*/
	glocal_mem_update[id].data.dev_srcId = glocal_mem_update[id].glocaldev_num;
	memcpy(&msg.ucData, &glocal_mem_update[id].data, ZJCT_STATUS_MSG_DATA_LEN);
	
	/* ������Ϣ*/						
	msg_len = msg.stHead.usLen +ZJCT_MNG_MSG_HEADER_LEN;
	
	Zjct_AdptMng_Socket_Send((uint8 *)&msg , msg_len, id);

	return DRV_OK;	
}
	
int32 Zjct_AdptMng_AutoSend_Open_GroupCall(void)
{
	ST_ZJCT_MNG_MSG msg ;
	uint32 msg_len = 0;
	
	memset(&msg, 0, sizeof(msg));
	
	/* �����Ϣͷ*/
	msg.stHead.ucType = ZJCT_MNG_TYPE_CONFIG;
	msg.stHead.ucDstAddr = ZJCT_USER_ADDR;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	
	/* �����Ϣ����*/
	msg.ucData[0] = ZJCT_DEV_ADDR_BRDCAST;
	msg.ucData[1] = glocal_mem_update[0].glocaldev_num;
	msg.ucData[2] = ZJCT_GROUPCALLMEM_START;
	
	memcpy(&msg.ucData[3], &groupcall_mem[1], groupcall_mem[0]);
		
	msg.stHead.usLen = groupcall_mem[0] + 3 ; 
	msg_len = msg.stHead.usLen +ZJCT_MNG_MSG_HEADER_LEN;
	
	/* ������Ϣ*/						
	//Zjct_AdptMng_Socket_Send((uint8 *)&msg , msg_len);
		
	return DRV_OK;	
}

static int32 Zjct_AdptMng_AutoSend_Close_GroupCall(void)
{
	ST_ZJCT_MNG_MSG msg ;
	uint32 msg_len = 0;
	
	memset(&msg, 0, sizeof(msg));
	
	/* �����Ϣͷ*/
	msg.stHead.ucType = ZJCT_MNG_TYPE_CONFIG;
	msg.stHead.ucDstAddr = ZJCT_USER_ADDR;
	msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	
	/* �����Ϣ����*/
	msg.ucData[0] = ZJCT_DEV_ADDR_BRDCAST;
	msg.ucData[1] = glocal_mem_update[0].glocaldev_num;
	msg.ucData[2] = ZJCT_GROUPCALLMEM_END;
	
	memcpy(&msg.ucData[3], &groupcall_mem[1], groupcall_mem[0]);
		
	msg.stHead.usLen = groupcall_mem[0] + 3 ; 
	msg_len = msg.stHead.usLen +ZJCT_MNG_MSG_HEADER_LEN;
	
	/* ������Ϣ*/						
	//Zjct_AdptMng_Socket_Send((uint8 *)&msg , msg_len);
		
	return DRV_OK;	
}

/*******************************************************************************
* Function: Zjct_AdtMng_QueryStatusMsgProc
*
* Description:  ����ϵͳ�и���ϵͳ���г�Ա������
*
* Inputs: buff -��Ϣ�г�ȥ����ͷ��������
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/

static int32 Zjct_Set_Bit(uint8 *buf, uint8 bit)
{		
	*((uint32 *)buf) |= 1 << bit;
	
	return DRV_OK;
}

static int32 Zjct_Clean_Bit(uint8 *buf, uint8 bit)
{
	*((uint32 *)buf) &= ~(1 << bit);
	
	return DRV_OK;
}

int32 Zjct_AdptMng_Update_MemList_OffLine_Msg(void)
{
	uint16 i = 0;
	uint16 offline[20] = {0};
	uint8 offline_cnt = 0;
	uint8 dev_type = CY1HH_TYPE_IDX;  /* Ŀǰֻ����Աһ�ź�*/
	uint8 index =0;//

	for(i = 0; i < ZJCT_DEV_INDX_MAX; i++)
	{
		index = ZJCT_DEV_INDX_MAX * dev_type + i;
		if(1 == gremote_mem_update[index].online)
		{
			if(time_cnt_100ms > gremote_mem_update[index].time_cnt)
			{
				if((time_cnt_100ms - gremote_mem_update[index].time_cnt) > PERIOD_4s)
				{
					int16 chan = -1;
					 gremote_mem_update[index].online = 0;
									 
					/* �ͷ�ռ��Ac491 channel */
					if(CHAN_CFG == g_chan_auto_cfg)
					{
						chan = gremote_mem_update[index].mem_chan_id; 
						free_one_channel(chan);
						gremote_mem_update[index].mem_chan_id = 0xFF;

						LOG("%s: member %d free chan %d\r\n", \
							__func__, index % ZJCT_DEV_INDX_MAX + 1, chan);						
					}
					
					 offline[offline_cnt] = index;
					 offline_cnt++;
					 
					 memset(&gremote_mem_update[index], 0x00, 
					 		sizeof(gremote_mem_update[index]));
					 memset(&gremote_mem_save[index], 0x00, 
					 		sizeof(gremote_mem_save[index]));
					 
					 if(offline_cnt >= 20)
					 {
						ERR("more than %d member offline 5s  \r\n", offline_cnt);
					 }
				}
			}
			else
			{     /* time_cnt ��������*/
				gremote_mem_update[index].time_cnt = time_cnt_100ms;
			}
		}
	}

//	Zjct_Socket_Send(g_ZjctAdptMngSockFd, &g_ZjctSeatMngSockAddr,  (uint8 *)offline,  offline_cnt * 2);
	/*����Ϣ֪ͨPC  ��*/
	for(i = 0; i < offline_cnt; i++)
	{
		Zjct_Clean_Bit((uint8 *)&online_mem, offline[i]%ZJCT_DEV_INDX_MAX);
		
		LOG("group %d, number %d offline \r\n", offline[i]/ZJCT_DEV_INDX_MAX,\
						 offline[i]%ZJCT_DEV_INDX_MAX + 1);	
		online_mem_cnt--;
	}

	if(offline_cnt)
	{
		DBG("online_mem = 0x%x, online_mem_cnt = %d\r\n",\
			online_mem, online_mem_cnt);
	}
	
	return DRV_OK;
}

int32 Zjct_AdptMng_Update_Radio_OffLine_Msg(void)
{
	uint8 i = 0;
	uint16 offline[20] = {0};
	uint8 offline_cnt = 0;

	for(i = 0; i < RADIO_NUM_MAX; i++)
	{
		if(1 == gremote_radio_update[i].online)
		{
			if(time_cnt_100ms > gremote_radio_update[i].time_cnt)
			{
				if((time_cnt_100ms - gremote_radio_update[i].time_cnt) > PERIOD_4s)
				{
					 gremote_radio_update[i].online = 0;
					 offline[offline_cnt] = i;
					 offline_cnt++;

					 memset(&gremote_radio_update[i], 0x00, 
					 		sizeof(gremote_radio_update[i]));
					 memset(&gremote_radio_save[i], 0x00, 
					 		sizeof(gremote_radio_save[i]));
				}
			}
			else
			{
				gremote_radio_update[i].time_cnt = time_cnt_100ms;
			}
		}

	}

	for(i = 0; i < offline_cnt; i++)
	{
		Zjct_Clean_Bit((uint8 *)&online_radio,
			offline[i] % RADIO_NUM_MAX);
		online_radio_cnt--;
		
		DBG("radio %d offline \r\n", i + 1);
		DBG("online_radio = 0x%x, online_radio_cnt = %d\r\n",\
			online_radio, online_radio_cnt);
	}

	return DRV_OK;
}

static int32 Zjct_AdptMng_Update_Radio_Online_Msg(uint8 *buf, uint32 len, uint32 ipaddr)
{
	ZJCT_RADIO_STATUS_MSG *msg = (ZJCT_RADIO_STATUS_MSG *)buf;
	int32 retval = 0;
	uint8 i = 0;
	uint8 index = 0;

	if((NULL == buf)||(0 == len))
	{
		ERR("%s param error\r\n", __func__);
		return DRV_ERR;
	}
	for(i = 0; i < RADIO_NUM_PER_DEV; i++)
	{
		index = msg->radio_mem[i].dev_srcId - 1;
		if(1 == gremote_radio_update[index].online)
		{
			retval = memcmp(&gremote_radio_update[index].mem_info,\
					&msg->radio_mem[i], 5);
			if(0 != retval)
			{
				memcpy(&gremote_radio_update[index].mem_info,\
					&msg->radio_mem[i], 5);
				memcpy(&gremote_radio_save[index].mem_info,\
					&gremote_radio_update[index].mem_info, 5);
				
				DBG("update the radio %d data \r\n", msg->radio_mem[i].dev_srcId);
				
				if((CY1HH_F400 | glocal_mem_update[0].glocaldev_num)==(gremote_radio_save[index].mem_info.dev_dscId)\
					&&(ZJCT_MODE_DTFS == gremote_radio_save[index].mem_info.dev_mode)\
					&&(ZJCT_MODE_KZDTFS == glocal_mem_update[0].data.dev_mode))
				{		
					glocal_mem_update[0].radio_ctl_status = RADIO_CTL_CONNECT;
					DBG("ZJCT_MODE_KZDTFS status\r\n");
				}
				else if((CY1HH_F400 | glocal_mem_update[0].glocaldev_num)==(gremote_radio_save[index].mem_info.dev_dscId)\
					&&(ZJCT_MODE_DTFS == gremote_radio_save[index].mem_info.dev_mode)\
					&&(ZJCT_MODE_GTFS == glocal_mem_update[0].data.dev_mode))
				{		
					glocal_mem_update[0].radio_gtctl_status = RADIO_CTL_CONNECT;
					DBG("ZJCT_MODE_GTFS status\r\n");
				}	
			}
		}
		else
		{

			memcpy(&gremote_radio_update[index].mem_info,\
				&msg->radio_mem[i], 5);
			memcpy(&gremote_radio_save[index].mem_info,\
				&gremote_radio_update[index].mem_info, 5);

			memcpy(&gremote_radio_update[index].hw_ver, &msg->hw_ver, 2);
			memcpy(&gremote_radio_update[index].sw_ver, &msg->sw_ver, 4);

			memcpy(&gremote_radio_update[index].ip_addr, &ipaddr, 4);
			gremote_radio_update[index].net_mask = 24;
			
			gremote_radio_update[index].online = 1;

			
			Zjct_Set_Bit((uint8 *)&online_radio, index%ZJCT_DEV_INDX_MAX);
			online_radio_cnt++;

			#if 0
			int8 chan = 0;
			chan = find_one_channel_for_radio(index + 1);
			if(DRV_ERR != chan)
			{
				g_chan[chan] = CHAN_USING;
				gremote_radio_update[index].chan_id = chan;
				LOG("radio %d get chan %d\r\n", \
					index % RADIO_NUM_MAX + 1, chan);
			}
			else
			{
				gremote_radio_update[index].chan_id = 0;
				ERR("%s: no free chan to radio %d\r\n", \
						__func__, index % RADIO_NUM_MAX + 1);
			}
			#endif
		}
		
		gremote_radio_update[index].time_cnt = time_cnt_100ms;
		
	}
	

	return DRV_OK;
}

/**********************************
return one channel can 
***********************************/
static int32 find_one_channel_for_member(void)
{
	int8 chan = DRV_ERR;
	uint8 i = 0;

	for(i = 0; i < AC491_MAX_CHAN_ALL; i++)
	{
		/* 3, 5 chan voice noise  */
		if((0 == i)||(5 == i)||(AC491_VOC_CHAN_FOR_RADIO == i)||(AC491_VOC_CHAN_FOR_ADD == i))
		{
			continue;
		}
		
		if(CHAN_FREE == g_chan[i])
		{
			chan = i;
			break;
		}
	}

	return chan;
}

static int32 free_one_channel(int32 chan)
{
    	if(chan < AC491_MAX_CHAN_ALL)
	{
	  	g_chan[chan] = CHAN_FREE;
	}

	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdptMng_Update_Mem1List_OnLine_Msg
*
* Description:  ����ϵͳ�и���ϵͳ���г�Ա������
*
* Inputs: buff 
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
static int32 Zjct_AdptMng_Update_Mem1List_OnLine_Msg(uint8 *buf, uint32 len, uint32 Ipaddr )
{
	if(ZJCT_STATUS_MSG_LEN != len)
	{
		ERR("status frame length error \r\n");
		return DRV_ERR;
	}

	ST_ZJCT_MNG_MSG msg;
	ST_ZJCT_MNG_MSG send_msg;
	
	int16 dev_type = 0, dev_idx = 0, retval = 0,online = -1, index =0;
	uint8 socket_send = 0;
	int16 chan = 0;
	static uint32 chan_dbg_cnt = 0;
	
	memcpy((uint8 *)&msg, buf, len);
	memset(&send_msg, 0, sizeof(send_msg));
	
	switch(msg.stHead.ucSrcAddr)
	{
		case ZJCT_DEV_ADDR_ZHZXH:
		
			/*�������Ŀ��ƺе�״̬��ѯ��־*/
			break;
		case ZJCT_DEV_ADDR_DTJKH:
			/* ��̨�ն�1~12��Ӧ���ն˵�ַΪ0x1~0xC*/

			break;
		case ZJCT_DEV_ADDR_CY1H:
			/* II���ն�1~32��Ӧ���ն˵�ַΪ0x1~0x20 */
			if((msg.ucData[1] < 1) ||(msg.ucData[1] > ZJCT_DEV_INDX_MAX))
			{
				ERR("CY1HH member number error!msg.ucData[1] = 0x%x\r\n", msg.ucData[1]);
				return DRV_ERR;
			}

			dev_type = CY1HH_TYPE_IDX;
			dev_idx = msg.ucData[1] - 1;			/* 0 ~ 31 */
			
			index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

			if (gremote_mem_update[index].Ipaddr != Ipaddr)
			{
				gremote_mem_update[index].Ipaddr = Ipaddr;
				gremote_mem_update[index].Netmask = 24;
			}
			
			if(1 == gremote_mem_update[index].online)
			{
			
				if((0xFF == gremote_mem_update[index].mem_chan_id )\
					&&(CHAN_CFG == g_chan_auto_cfg))
				{
					/* Ϊ��Աһ�źз���һ��ac491 channel */
					chan = find_one_channel_for_member();
					if(DRV_ERR != chan)
					{
						g_chan[chan] = CHAN_USING;
						gremote_mem_update[index].mem_chan_id = chan;
						LOG("member %d get chan %d index = %d\r\n", index % ZJCT_DEV_INDX_MAX + 1, \
							gremote_mem_update[index].mem_chan_id, index);
					}
					else
					{	
						gremote_mem_update[index].mem_chan_id = 0xFF;
						chan_dbg_cnt++;
						if(30 <= chan_dbg_cnt)
						{
							chan_dbg_cnt = 0;
							ERR("%s: no free chan to member %d\r\n", \
									__func__, index % ZJCT_DEV_INDX_MAX + 1);
						}
					}
				}			
				
				retval = memcmp((uint8 *)(&gremote_mem_update[index].data), &msg.ucData[0], \
								ZJCT_STATUS_MSG_UPDATE_DATA_LEN);
				if(0 != retval)
				{
				
					memcpy((uint8 *)(&gremote_mem_update[index].data), &msg.ucData[0], \
								ZJCT_STATUS_MSG_DATA_LEN);
					
					DBG("update group %d, number %d data \r\n", \
						index/ZJCT_DEV_INDX_MAX, (index%ZJCT_DEV_INDX_MAX +1));
									
					switch(gremote_mem_save[index].data.dev_mode)
					{
						case ZJCT_MODE_QHFS:
							/* ǿ������*/
							if(ZJCT_MODE_QHFS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_QHFS copy the gremote_mem_update data to gremote_mem_save data \r\n");

								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data,\
											ZJCT_STATUS_MSG_DATA_LEN);

								Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update[0].data,\
											ZJCT_STATUS_MSG_DATA_LEN);

								glocal_mem_update[0].force_call_srcId = 0;	

								/* ֪ͨ��������������*/
								memset(&send_msg, 0, sizeof(send_msg));
								send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
								send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
								send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
								
								send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
								send_msg.ucData[DANHU_MSG_BYTE] = FORCECALLIN_END;
								//send_msg.ucData[3] = ZJCT_MODE_QHJS;
								send_msg.ucData[3] = ZJCT_MODE_QHFS;
								socket_send = 1;
														
								DBG("end one force call\r\n");
							}
							
							break;
						case ZJCT_MODE_QHJS:
							/* ǿ������*/
							if(ZJCT_MODE_QHJS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_QHJS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
							break;
						case ZJCT_MODE_CTZH:
							/* ��ͨ���*/
							if(ZJCT_MODE_CTZH != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_CTZH copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
							break;
						case ZJCT_MODE_DH:  
							/* ����*/	
							
							/*��⵽�Զ��������Ҷϵ绰��֪ͨSeat �绰*/
							if (((CY1HH_F400 | glocal_mem_update[0].glocaldev_num) == gremote_mem_save[index].data.dev_dscId)\
							&&((CY1HH_F400 | glocal_mem_update[0].glocaldev_num) != gremote_mem_update[index].data.dev_dscId)\
							&&(ZJCT_MODE_QHJS != gremote_mem_update[index].data.dev_mode)\
							&&(msg.ucData[1] == glocal_mem_update[0].callIn_id))  
							{
								/* ֪ͨ��������������*/
								memset(&send_msg, 0, sizeof(send_msg));
								send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
								send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
								send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
								
								send_msg.ucData[DANHU_MSG_BYTE] = DANHU_DISCONNECT_PASSIVE;	/*0x05*/
								send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
								send_msg.ucData[3] = ZJCT_MODE_DH;
								*(uint16 *)(&send_msg.ucData[0x04]) = *(uint16 *)(&msg.ucData[0x04]);
								send_msg.ucData[1] = msg.ucData[1];

								LOG("active_release = %d\r\n", glocal_mem_update[0].active_release);

								if(0 == glocal_mem_update[0].active_release)
								{
									socket_send = 1;
								}
								else
								{
									glocal_mem_update[0].active_release = 0;
								}
							
								Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update[0].data, \
										ZJCT_STATUS_MSG_DATA_LEN);
								
								glocal_mem_update[0].callIn = 0;
								glocal_mem_update[0].connect = 0;
								glocal_mem_update[0].callIn_id = 0x00;
								
								
								if(FMQ_CTRL_OFF != buzzer_status)
								{
									Buzzer_Control(FMQ_CTRL_OFF);
									buzzer_status = FMQ_CTRL_OFF;
								}
								
								DBG("auto hangup index = %d\r\n", index);
										
							}
							
							if(ZJCT_MODE_DH != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_DH copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
															
							break;
						case ZJCT_MODE_CNGB:
							/* ���ڹ㲥*/
							if(ZJCT_MODE_CNGB != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_CNGB copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
							break;	
						case ZJCT_MODE_KZDTFS:
							/* ���Ƶ�̨����*/
							if(ZJCT_MODE_KZDTFS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_KZDTFS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
							break;
						case ZJCT_MODE_KZDTJS:
							/* ���Ƶ�̨����*/
							if(ZJCT_MODE_KZDTJS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_KZDTJS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}		
							break;
						case ZJCT_MODE_GTFS:
							/* ��������*/
							if(ZJCT_MODE_GTFS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_GTFS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}	
							break;
						case ZJCT_MODE_GTJS:
							/* ��������*/
							if(ZJCT_MODE_GTJS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_GTJS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}	
							break;
						case ZJCT_MODE_BHFS:
							/* ���Ž�������*/
							if(ZJCT_MODE_BHFS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_BHFS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}								
							break;
						case ZJCT_MODE_BHJS:
							/* ���Ž�������*/
							if(ZJCT_MODE_BHJS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_BHJS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}							
							break;
						default:
							ERR("case ZJCT_DEV_ADDR_CY1H param error !gremote_mem_save[index].data.dev_mode = 0x%x\r\n",\
									gremote_mem_save[index].data.dev_mode);
							break;	
					}
				
					switch(msg.ucData [3])
					{
						case ZJCT_MODE_QHFS:
							/* ǿ������*/

							glocal_mem_update[0].data.dev_mode = ZJCT_MODE_CNGB;
							glocal_mem_update[0].data.dev_dscId  = ZJCT_BROADCASE_ADDR;
							
							Zjct_Local_Mem_Data_Push((uint8 *)&glocal_mem_update[0].data,\
														ZJCT_STATUS_MSG_DATA_LEN);
							
							glocal_mem_update[0].data.dev_mode = ZJCT_MODE_QHJS;
							glocal_mem_update[0].data.dev_dscId = *((uint16 *)&(msg.ucData[0x04]));
							glocal_mem_update[0].force_call_srcId = msg.ucData[1];

							if(1 == gauto_send_cfg_flg)
							{
								gauto_send_cfg_flg = 0;
							}

							if(1 == gdanhu_timeout_flg)
							{
								gdanhu_timeout_flg = 0;
							}

							if(0 != glocal_mem_update[0].inc_callIn)
							{
								//BoardSem_Call_HandUp(buf, len);
								glocal_mem_update[0].inc_callIn = 0;
								glocal_mem_update[0].inc_connect = ZJCT_INC_DISCONNECT;
							}

							if(0 != glocal_mem_update[0].connect)
							{
								glocal_mem_update[0].connect = 0;
								glocal_mem_update[0].callIn_id = 0x00;
							}	

							if(0 != glocal_mem_update[0].callIn)
							{
								glocal_mem_update[0].callIn = 0;
							}								
							
							/* ֪ͨ��������������*/
							memset(&send_msg, 0, sizeof(send_msg));
							send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
							send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
							send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;

							send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
							send_msg.ucData[DANHU_MSG_BYTE] = FORCECALLIN;
							//send_msg.ucData[3] = ZJCT_MODE_QHJS;
							send_msg.ucData[3] = ZJCT_MODE_QHFS;
							socket_send = 1;

							DBG("one force call in \r\n");							
							break;
						case ZJCT_MODE_QHJS:
							/* ǿ������*/
							break;
						case ZJCT_MODE_CTZH:
							/* ��ͨ���*/
							break;
						case ZJCT_MODE_DH:   	/* ����*/							
							if((CY1HH_F400 |glocal_mem_update[0].glocaldev_num ) == *(uint16 *)(&msg.ucData[0x04]))
							{
								if((CY1HH_F400|msg.ucData[1]) != glocal_mem_update[0].data.dev_dscId) /* ���� */
								{
									if((ZJCT_MODE_DH != glocal_mem_update[0].data.dev_mode)\
									&&(ZJCT_MODE_QHFS != glocal_mem_update[0].data.dev_mode)\
									&&(ZJCT_MODE_QHJS != glocal_mem_update[0].data.dev_mode)\
									&&(ZJCT_MODE_CTZH != glocal_mem_update[0].data.dev_mode)\
									&&(ZJCT_MODE_GTFS != glocal_mem_update[0].data.dev_mode)\
									&&(ZJCT_MODE_BHFS != glocal_mem_update[0].data.dev_mode)\
									&&(ZJCT_MODE_BHJS != glocal_mem_update[0].data.dev_mode)\
									&&(ZJCT_MODE_DTFS != glocal_mem_update[0].data.dev_mode)\
									&&(0 == glocal_mem_update[0].callIn)\
									&&(0 == glocal_mem_update[0].inc_callIn))
									{
										/*����adatper ����*/
										glocal_mem_update[0].callIn = 1;
										glocal_mem_update[0].connect = 0;

										glocal_mem_update[0].callIn_id = msg.ucData[1];
										
										/* ֪ͨ��������������*/
										memset(&send_msg, 0, sizeof(send_msg));
										send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;	
										send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
										send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;

										send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
										send_msg.ucData[3] = ZJCT_MODE_DH;
										
										*(uint16 *)(&send_msg.ucData[0x04]) = *(uint16 *)(&msg.ucData[0x04]);
										send_msg.ucData[1] = msg.ucData[1];
										
										//*(uint16 *)(&send_msg.ucData[0x04]) = CY1HH_F400 |msg.ucData[1];	
										
										send_msg.ucData[DANHU_MSG_BYTE] = DANHU_CALLIN;
										socket_send = 1;
										Buzzer_Control(FMQ_CTRL_FAST);
										buzzer_status = FMQ_CTRL_FAST;

										DBG("have a call in index = %d\r\n", index);
									}
								}
								else if((CY1HH_F400|msg.ucData[1]) == glocal_mem_update[0].data.dev_dscId) 		
								{	
									/* ����adatper  ��Seat ����Ӧ��*/

									/* ֪ͨ��������������*/
									memset(&send_msg, 0, sizeof(send_msg));
									send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
									send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
									send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;

									send_msg.ucData[DANHU_MSG_BYTE] = DANHU_CONNECT;
									send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
									send_msg.ucData[3] = ZJCT_MODE_DH;
									*(uint16 *)(&send_msg.ucData[0x04]) = *(uint16 *)(&msg.ucData[0x04]);
									send_msg.ucData[1] = msg.ucData[1];
									//*(uint16 *)(&send_msg.ucData[0x04]) = CY1HH_F400 |msg.ucData[1];
									socket_send = 1;
									
									glocal_mem_update[0].connect = 1;

									gdanhu_timeout_flg = 0;
									
									DBG("have been connected index = %d\r\n", index);
								}
								else
								{
									printf("ZJCT_MODE_DH error CY1HH_F400|msg.ucData[1] = 0x%x\r\n", \
										CY1HH_F400|msg.ucData[1]);
								}
							}
							break;
						case ZJCT_MODE_CNGB:
							/* ���ڹ㲥*/						
							break;	
						case ZJCT_MODE_KZDTFS:
							/* ���Ƶ�̨����*/						
							break;
						case ZJCT_MODE_KZDTJS:
							/* ���Ƶ�̨����*/							
							break;
						case ZJCT_MODE_GTFS:
							/* ��������*/							
							break;
						case ZJCT_MODE_GTJS:
							/* ��������*/							
							break;
						case ZJCT_MODE_BHFS:
							/* ���Ž�������*/							
							break;
						case ZJCT_MODE_BHJS:
							/* ���Ž�������*/
							break;
						default:
							ERR("case ZJCT_DEV_ADDR_CY1H param error !msg.ucData [3] = 0x%x\r\n",\
									msg.ucData [3]);
							break;		
					}
				
				}
			}
			else 
			{
				memcpy((uint8 *)(&gremote_mem_update[index].data), \
						&msg.ucData[0], ZJCT_STATUS_MSG_DATA_LEN);

				memcpy((uint8 *)(&gremote_mem_save[index].data), \
						&msg.ucData[0], ZJCT_STATUS_MSG_DATA_LEN);

				gremote_mem_update[index].online = 1;
				online = index;	

				memcpy(&gremote_mem_update[index].Ipaddr, &Ipaddr, 4);
				gremote_mem_update[index].Netmask = 24;
				
				if(CHAN_CFG == g_chan_auto_cfg)
				{
					/* Ϊ��Աһ�źз���һ��ac491 channel */
					chan = find_one_channel_for_member();
					if(DRV_ERR != chan)
					{
						g_chan[chan] = CHAN_USING;
						gremote_mem_update[index].mem_chan_id = chan;
					#ifdef AC491_CHAN_TEST
						if(0x08 == msg.ucData[1])
						{
							gremote_mem_update[index].mem_chan_id = g_dbg_chan;
						}
					#endif						
						LOG("member %d get chan %d index = %d\r\n", index % ZJCT_DEV_INDX_MAX + 1, \
							gremote_mem_update[index].mem_chan_id, index);
					}
					else
					{
						gremote_mem_update[index].mem_chan_id = 0xFF;

						gremote_mem_update[index].mem_save = 0;
						gremote_mem_update[index].mem_get = 0;
						
						ERR("%s: no free chan to member %d\r\n", \
								__func__, index % ZJCT_DEV_INDX_MAX + 1);
					}
				}
				else
				{
						//g_chan[dev_idx + 1] = CHAN_USING;
						//gremote_mem_update[index].mem_chan_id = dev_idx + 1;		
				}
			}
			
			gremote_mem_update[index].time_cnt = time_cnt_100ms;
				
			if(1 == socket_send)
			{
				Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);
			}

			if(-1 != online)
			 {
			 //	Zjct_Socket_Send(g_ZjctAdptMngSockFd, &g_ZjctSeatMngSockAddr,  &online,  sizeof(online));

			 	Zjct_Set_Bit((uint8 *)&online_mem, online%ZJCT_DEV_INDX_MAX);
			 	online_mem_cnt++;
				
				DBG("add one new member group %d, numer %d \r\n", \
							online/ZJCT_DEV_INDX_MAX, (online%ZJCT_DEV_INDX_MAX +1));
				DBG("online_mem = 0x%x, online_mem_cnt = %d\r\n",\
						online_mem, online_mem_cnt);
			 	/*����Ϣ֪ͨPC  ��*/
			 }
			break;
		case ZJCT_DEV_ADDR_CY2H:
			/* ��Ա2  �ź�*/

			break;
		case ZJCT_DEV_ADDR_YHJKH:
			/* �û��ӿں� */
			break;	
		case ZJCT_DEV_ADDR_YHKZH:
			/* �û���չ�� */
			break;	
		case ZJCT_DEV_ADDR_ZY1H:
			/* ��Ա1�ź�*/
			break;		
		default:
			ERR("%s: param error !msg.stHead.ucSrcAddr = 0x%x\r\n",\
					__func__, msg.stHead.ucSrcAddr);
			break;
	}
			
	return DRV_OK;
}

int32 Zjct_AdptMng_Update_Mem2List_OnLine_Msg(uint8 *buf, uint32 len, uint32 Ipaddr )
{
	if(ZJCT_STATUS_MSG_LEN != len)
	{
		ERR("status frame length error \r\n");
		return DRV_ERR;
	}

	ST_ZJCT_MNG_MSG msg;
	ST_ZJCT_MNG_MSG send_msg;
	
	int16 dev_type = 0, dev_idx = 0, retval = 0,online = -1, index =0;
	uint8 socket_send = 0;
	int16 chan = 0;
	static uint32 chan_dbg_cnt = 0;
	
	memcpy((uint8 *)&msg, buf, len);
	memset(&send_msg, 0, sizeof(send_msg));
	
	switch(msg.stHead.ucSrcAddr)
	{
		case ZJCT_DEV_ADDR_ZHZXH:
		
			/*�������Ŀ��ƺе�״̬��ѯ��־*/
			break;
		case ZJCT_DEV_ADDR_DTJKH:
			/* ��̨�ն�1~12��Ӧ���ն˵�ַΪ0x1~0xC*/
			break;
		case ZJCT_DEV_ADDR_CY1H:
			/* ��Ա1  �ź�*/			
			break;
		case ZJCT_DEV_ADDR_CY2H:
			/* ��Ա2  �ź�*/
			/* II���ն�1~32��Ӧ���ն˵�ַΪ0x1~0x20 */
			if((msg.ucData[1] < 1) ||(msg.ucData[1] > ZJCT_DEV_INDX_MAX))
			{
				ERR("CY1HH member number error!msg.ucData[1] = 0x%x\r\n", msg.ucData[1]);
				return DRV_ERR;
			}

			dev_type = CY2HH_TYPE_IDX;
			dev_idx = msg.ucData[1] - 1;			/* 0 ~ 31 */
			
			index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

			if (gremote_mem_update[index].Ipaddr != Ipaddr)
			{
				gremote_mem_update[index].Ipaddr = Ipaddr;
				gremote_mem_update[index].Netmask = 24;
			}
			
			if(1 == gremote_mem_update[index].online)
			{
			
				if((0xFF == gremote_mem_update[index].mem_chan_id )\
					&&(CHAN_CFG == g_chan_auto_cfg))
				{
					/* Ϊ��Աһ�źз���һ��ac491 channel */
					chan = find_one_channel_for_member();
					if(DRV_ERR != chan)
					{
						g_chan[chan] = CHAN_USING;
						gremote_mem_update[index].mem_chan_id = chan;
						LOG("member %d get chan %d index = %d\r\n", index % ZJCT_DEV_INDX_MAX + 1, \
							gremote_mem_update[index].mem_chan_id, index);
					}
					else
					{
						gremote_mem_update[index].mem_chan_id = 0xFF;
						chan_dbg_cnt++;
						if(30 <= chan_dbg_cnt)
						{
							chan_dbg_cnt = 0;
							
							ERR("%s: no free chan to member %d\r\n", \
									__func__, index % ZJCT_DEV_INDX_MAX + 1);
						}
					}
				}			
				
				retval = memcmp((uint8 *)(&gremote_mem_update[index].data), &msg.ucData[0], \
								ZJCT_STATUS_MSG_UPDATE_DATA_LEN);
				if(0 != retval)
				{
					memcpy((uint8 *)(&gremote_mem_update[index].data), &msg.ucData[0], \
								ZJCT_STATUS_MSG_DATA_LEN);
					
					DBG("update group %d, number %d data \r\n", \
						index/ZJCT_DEV_INDX_MAX, (index%ZJCT_DEV_INDX_MAX +1));
									
					switch(gremote_mem_save[index].data.dev_mode)
					{
						case ZJCT_MODE_QHFS:
							/* ǿ������*/
							if(ZJCT_MODE_QHFS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_QHFS copy the gremote_mem_update data to gremote_mem_save data \r\n");

								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data,\
											ZJCT_STATUS_MSG_DATA_LEN);

								Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update[0].data,\
											ZJCT_STATUS_MSG_DATA_LEN);

								glocal_mem_update[0].force_call_srcId = 0;	

								/* ֪ͨ��������������*/
								memset(&send_msg, 0, sizeof(send_msg));
								send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
								send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
								send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY2H;
								
								send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
								send_msg.ucData[DANHU_MSG_BYTE] = FORCECALLIN_END;
								//send_msg.ucData[3] = ZJCT_MODE_QHJS;
								send_msg.ucData[3] = ZJCT_MODE_QHFS;
								socket_send = 1;
														
								DBG("end one force call\r\n");
							}
							break;
						case ZJCT_MODE_QHJS:
							/* ǿ������*/
							if(ZJCT_MODE_QHJS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_QHJS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
							break;
						case ZJCT_MODE_CTZH:
							/* ��ͨ���*/
							if(ZJCT_MODE_CTZH != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_CTZH copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
							break;
						case ZJCT_MODE_DH:  
							/* ����*/	
							if(ZJCT_MODE_DH != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_DH copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
															
							break;
						case ZJCT_MODE_CNGB:
							/* ���ڹ㲥*/
							if(ZJCT_MODE_CNGB != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_CNGB copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
							break;	
						case ZJCT_MODE_KZDTFS:
							/* ���Ƶ�̨����*/
							if(ZJCT_MODE_KZDTFS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_KZDTFS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}
							break;
						case ZJCT_MODE_KZDTJS:
							/* ���Ƶ�̨����*/
							if(ZJCT_MODE_KZDTJS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_KZDTJS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}		
							break;
						case ZJCT_MODE_GTFS:
							/* ��������*/
							if(ZJCT_MODE_GTFS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_GTFS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}	
							break;
						case ZJCT_MODE_GTJS:
							/* ��������*/
							if(ZJCT_MODE_GTJS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_GTJS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}	
							break;
						case ZJCT_MODE_BHFS:
							/* ���Ž�������*/
							if(ZJCT_MODE_BHFS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_BHFS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}								
							break;
						case ZJCT_MODE_BHJS:
							/* ���Ž�������*/
							if(ZJCT_MODE_BHJS != gremote_mem_update[index].data.dev_mode)
							{
								DBG("ZJCT_MODE_BHJS copy the gremote_mem_update data to gremote_mem_save data \r\n");
								memcpy(&gremote_mem_save[index].data, &gremote_mem_update[index].data, \
											ZJCT_STATUS_MSG_DATA_LEN);
							}							
							break;
						default:
							ERR("case ZJCT_DEV_ADDR_CY1H param error !gremote_mem_save[index].data.dev_mode = 0x%x\r\n",\
									gremote_mem_save[index].data.dev_mode);
							break;	
					}
				
					switch(msg.ucData [3])
					{
						case ZJCT_MODE_QHFS:
							/* ǿ������*/
							glocal_mem_update[0].data.dev_mode = ZJCT_MODE_CNGB;
							glocal_mem_update[0].data.dev_dscId  = ZJCT_BROADCASE_ADDR;
							
							Zjct_Local_Mem_Data_Push((uint8 *)&glocal_mem_update[0].data,\
														ZJCT_STATUS_MSG_DATA_LEN);
							
							glocal_mem_update[0].data.dev_mode = ZJCT_MODE_QHJS;
							glocal_mem_update[0].data.dev_dscId = *((uint16 *)&(msg.ucData[0x04]));
							glocal_mem_update[0].force_call_srcId = msg.ucData[1];

							/* ֪ͨ��������������*/
							memset(&send_msg, 0, sizeof(send_msg));
							send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
							send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
							send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY2H;

							send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
							send_msg.ucData[DANHU_MSG_BYTE] = FORCECALLIN;
							//send_msg.ucData[3] = ZJCT_MODE_QHJS;
							send_msg.ucData[3] = ZJCT_MODE_QHFS;
							socket_send = 1;

							DBG("one force call in \r\n");							
							break;
						case ZJCT_MODE_QHJS:
							/* ǿ������*/
							break;
						case ZJCT_MODE_CTZH:
							/* ��ͨ���*/
							break;
						case ZJCT_MODE_DH:   	/* ����*/							
							break;
						case ZJCT_MODE_CNGB:
							/* ���ڹ㲥*/						
							break;	
						case ZJCT_MODE_KZDTFS:
							/* ���Ƶ�̨����*/						
							break;
						case ZJCT_MODE_KZDTJS:
							/* ���Ƶ�̨����*/							
							break;
						case ZJCT_MODE_GTFS:
							/* ��������*/							
							break;
						case ZJCT_MODE_GTJS:
							/* ��������*/							
							break;
						case ZJCT_MODE_BHFS:
							/* ���Ž�������*/							
							break;
						case ZJCT_MODE_BHJS:
							/* ���Ž�������*/
							break;
						default:
							ERR("case ZJCT_DEV_ADDR_CY1H param error !msg.ucData [3] = 0x%x\r\n",\
									msg.ucData [3]);
							break;		
					}
				}
			}
			else 
			{
				memcpy((uint8 *)(&gremote_mem_update[index].data), \
						&msg.ucData[0], ZJCT_STATUS_MSG_DATA_LEN);

				memcpy((uint8 *)(&gremote_mem_save[index].data), \
						&msg.ucData[0], ZJCT_STATUS_MSG_DATA_LEN);

				gremote_mem_update[index].online = 1;
				online = index;	

				memcpy(&gremote_mem_update[index].Ipaddr, &Ipaddr, 4);
				gremote_mem_update[index].Netmask = 24;
				
				if(CHAN_CFG == g_chan_auto_cfg)
				{
					/* Ϊ��Աһ�źз���һ��ac491 channel */
					chan = find_one_channel_for_member();
					if(DRV_ERR != chan)
					{
						g_chan[chan] = CHAN_USING;
						gremote_mem_update[index].mem_chan_id = chan;
					#ifdef AC491_CHAN_TEST
						if(0x08 == msg.ucData[1])
						{
							gremote_mem_update[index].mem_chan_id = g_dbg_chan;
						}
					#endif						
						LOG("member %d get chan %d index = %d\r\n", index % ZJCT_DEV_INDX_MAX + 1, \
							gremote_mem_update[index].mem_chan_id, index);
					}
					else
					{
						gremote_mem_update[index].mem_save = 0;
						gremote_mem_update[index].mem_get = 0;
					
						gremote_mem_update[index].mem_chan_id = 0xFF;
						ERR("%s: no free chan to member %d\r\n", \
								__func__, index % ZJCT_DEV_INDX_MAX + 1);
					}
				}
				else
				{
						//g_chan[dev_idx + 1] = CHAN_USING;
						//gremote_mem_update[index].mem_chan_id = dev_idx + 1;		
				}
			}
			
			gremote_mem_update[index].time_cnt = time_cnt_100ms;
				
			if(1 == socket_send)
			{
				Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);
			}

			if(-1 != online)
			 {
			 	//Zjct_Set_Bit((uint8 *)&online_mem, online%ZJCT_DEV_INDX_MAX);
			 	//online_mem_cnt++;
				
				DBG("add one new member group %d, numer %d \r\n", \
							online/ZJCT_DEV_INDX_MAX, (online%ZJCT_DEV_INDX_MAX +1));
				DBG("online_mem = 0x%x, online_mem_cnt = %d\r\n",\
						online_mem, online_mem_cnt);
			 	/*����Ϣ֪ͨPC  ��*/
			 }			
			break;
		case ZJCT_DEV_ADDR_YHJKH:
			/* �û��ӿں� */
			break;	
		case ZJCT_DEV_ADDR_YHKZH:
			/* �û���չ�� */
			break;	
		case ZJCT_DEV_ADDR_ZY1H:
			/* ��Ա1�ź�*/
			break;		
		default:
			ERR("%s: param error !msg.stHead.ucSrcAddr = 0x%x\r\n",\
					__func__, msg.stHead.ucSrcAddr);
			break;
	}
			
	return DRV_OK;
}

static int32 Zjct_AdptMng_Param_Update_Ack(uint8 *buf, uint32 len)
{
	DBG("Zjct_AdptMng_Param_Update_Ack \r\n");
	
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;

	uint8 temp = 0;
	
	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}

	/*�������·�Ӧ��֡*/

	msg->stHead.ucType = ZJCT_MNG_TYPE_PARAMUPADATEACK;
	temp = msg->stHead.ucSrcAddr;
	msg->stHead.ucSrcAddr = msg->stHead.ucDstAddr;
	msg->stHead.ucDstAddr = temp;

	temp = msg->ucData[0];
	msg->ucData[0] = msg->ucData[1];
	msg->ucData[1] = temp;

	msg->ucData[2] = 0x00;
	msg->ucData[3] = 0x00;
	msg->ucData[4] = 0x00;

	msg->ucData[5] = 0x00;
	msg->ucData[6] = 0x02;

	msg->ucData[7] = 0x00;
	msg->ucData[8] = 0x00;
	
	msg->stHead.usLen = 0x09;

	//Zjct_AdptMng_Socket_Send((uint8 *)msg, 9 + 5);

	return DRV_OK;
}


static int32 Zjct_AdptMng_Param_Update(uint8 *buf, uint32 len)
{
	printf("Zjct_AdptMng_Param_Update \r\n");
	
	uint16 length = 0, temp_cnt = 0;
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;
	
	length = *(uint16 *)(&(msg->ucData[5]));
	DBG("Zjct_AdptMng_Param_Update length = %d\r\n", length);
	if(msg->ucData[10] != glocal_mem_update[0].glocaldev_num)
	{
		DBG("Zjct_AdptMng_Param_Update failed msg->ucData[10]= 0x%x\r\n", \
				msg->ucData[10]);
		return DRV_OK;
	}

	temp_cnt = strlen((char *)(&msg->ucData[11]));

	Zjct_AdptMng_Param_Update_Ack(buf, len);
	
	return DRV_OK;
}

static int32 Zjct_AdptMng_Msg_Process(uint8 *buf, uint32 len, uint8 id, uint32 Ipaddr)
{
	if ((NULL == buf) ||(0x00 == len ))
	{
		ERR("Zjct_AdptMng_Msg_Process buf or len error"
			"buf = 0x%x, len = %d\r\n", buf, len);
		return DRV_ERR;
	}

	uint8 ret = 0;

	//ST_ZJCT_MNG_MSG Msg, *msg = NULL;
	//memset(&Msg, 0, sizeof(Msg));
	//memcpy(&Msg, buf, len);
	//msg = &Msg;

	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;
	
	switch (msg->stHead.ucType)
	{	
		case ZJCT_MNG_TYPE_DEV_STATUS_INFO: 
			switch(msg->stHead.ucSrcAddr)
			{
				/*���ƺ�״̬֡*/
				case ZJCT_DEV_ADDR_ZDZXH:

					break;
				/*��̨��״̬֡*/
				case ZJCT_DEV_ADDR_DTJKH:
					if((ZJCT_STATUS_MSG_LEN - 2)== len)
					{
						Zjct_AdptMng_Update_Radio_Online_Msg(buf, len, Ipaddr);
					}
					break;
				/*��Ա��1  �ź�״̬֡*/
				case ZJCT_DEV_ADDR_CY1H:
					if(ZJCT_STATUS_MSG_LEN == len)
					{
						Zjct_AdptMng_Update_Mem1List_OnLine_Msg(buf, len, Ipaddr);	
					}		
					break;
				/*��Ա��2  �ź�״̬֡*/
				case ZJCT_DEV_ADDR_CY2H:					
					if(ZJCT_STATUS_MSG_LEN == len)
					{
						//Zjct_AdptMng_Update_Mem2List_OnLine_Msg(buf, len, Ipaddr);	
					}					
					break;					
				default:
					break;
			}
			break;		
		case ZJCT_MNG_TYPE_REGREQACK:
			if(DRV_OK != Zjct_AdptMng_SendRegisterNetwork_Req_Ack(buf, len))
			{			
				ERR("Zjct_AdptMng_SendRegisterNetwork_Req_Ack failed\r\n");
				return DRV_ERR;
			}
			DBG("Zjct_AdptMng_SendRegisterNetwork_Req OK! \r\n");

			SET_NETREG_FLG(id);

		#if 0	
			if(DHCP_OK != dhcp_status)
			{
				LOG("%s: reboot for getting IP addr from"
					" DHCP server\r\n", __func__);
				system_reboot();
			}
		#endif	
		
			//Zjct_AdptMng_SendParamConfig_Req();
			break;				
		case ZJCT_MNG_TYPE_PARAMCFG:
			if(DRV_OK != (Zjct_AdptMng_SendParamConfig_Proc(buf, len, id)))
			{
				ERR("Zjct_AdptMng_SendParamConfig_Proc failed \r\n");
				return DRV_ERR;
			}
			
			SET_PARAMREQ_FLG(id);
			
			break;
		case ZJCT_MNG_TYPE_CONFIG:
			ret = Zjct_AdptMng_Receive_Param_Set(buf, len);
			if (DRV_OK != ret)
			{
				ERR("Zjct_AdptMng_Receive_Param_Set failed \r\n");
				return DRV_ERR;
			}
			break;	
		case ZJCT_MNG_TYPE_CONFIG_ACK:

			ret = Zjct_AdptMng_Receive_Param_Set_Ack(buf, len);
			if (DRV_OK != ret)
			{
				ERR("Zjct_AdptMng_Receive_Param_Set_Ack failed \r\n");
				return DRV_ERR;
			}
			break;
		case ZJCT_MNG_TYPE_PARAMUPADATE:
			/*�����·�֡*/
			ret = Zjct_AdptMng_Param_Update(buf, len);
			if(DRV_OK != ret)
			{
				ERR("Zjct_AdptMng_Param_update failed \r\n");
				return DRV_ERR;
			}
			break;
		default:
/*
			ERR("Zjct_AdptMng_Msg_Process param error! msg->stHead.ucType =0x%x\r\n", \
					msg->stHead.ucType);
*/
			return DRV_ERR;
			break;
	}

	return DRV_OK;
}


/*=======================================
ָ��ͨ�Ű�ͨ����ͨ��������INC ֮��ͨ��
=======================================*/
/*******************************************************************************
* Function: Zjct_AdptVoc_Socket_init
*
* Description: ָ��ͨ�Ű��복ͨ������֮���������ͨ��Socket��ʼ��.
			��Socket�����շ���ͨϵͳ�����PCM���������շ� ��
			Socket IP��ַ��ͨ��DHCP��ȡ�� ��ǰ�������ļ��ж�ȡ��
*
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	zjct�Ļ������Թ㲥��ʽ���͸�II�ͣ�
			II�ͽ���socket�����ù㲥���ԡ�
*******************************************************************************/
static int32 Zjct_AdptVoc_Socket_init(void)
{
	struct sockaddr_in my_addr = {0};

	int broadcast_flag = 1;

		/* ���չ㲥����*/
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(26113);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if ((g_ZjctAdptVocSockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ERR("create Zjct_AdptVoc_Socket failed \r\n");
		return DRV_ERR;
	}

	if (DRV_OK != bind(g_ZjctAdptVocSockFd, (struct sockaddr *)&my_addr, sizeof(my_addr)))
	{
		ERR("Bind Zjct_AdptVoc_Socket failed.\r\n");
		close(g_ZjctAdptVocSockFd);
		return DRV_ERR;
	}

	/* �����ܷ��ͥxx.xx.xx.255�Ĺ㲥����*/
	setsockopt(g_ZjctAdptVocSockFd, SOL_SOCKET, SO_BROADCAST, &broadcast_flag, sizeof(broadcast_flag));
				
	return DRV_OK;
}
/*******************************************************************************
* Function: Zjct_AdptVoc_RxThread
*
* Description: ָ��ͨ�Ű崦�����Գ�ͨ������(����UDP�˿ں�0x6601) 
			��PCM�������ݡ�
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
void Zjct_AdptVoc_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;	
	uint32 len = 0;
	//uint8 *buf = temp_buf;
	ST_ZJCT_MNG_MSG *msg = NULL;

	uint16 dev_type = 0, dev_idx = 0, index =0;
	dev_type = CY1HH_TYPE_IDX;
	uint8 id = 0;
	uint8 process_flg = 1;
	LOG("Zjct_AdptVoc_RxThread start \r\n");

	len = sizeof(remote_addr);
	
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		
		if((Rec_Len = recvfrom(g_ZjctAdptVocSockFd, (void *)buf, ZJCT_MAX_DATA_LEN_600, \
			0, (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			process_flg = 1;

			if(ZJCT_PKGVOC_LEN_328 == Rec_Len)
			{
				/*���˵��Լ������Ĺ㲥��*/
				for(id = 0; id < CHETONG_SEAT_MAX; id++)
				{
					if(htonl(II_Adpt_local[id].ipaddr) == remote_addr.sin_addr.s_addr)
					{
						process_flg = 0;
						break;
					}
				}

				if(0 == process_flg)
				{
					continue;
				}
				/* �ж��Ƿ�Ϊ128k PCM  ����֡*/
				if ( ZJCT_MNG_TYPE_PCM128K != *buf)
				{
					ERR("It is not PCM frame buf[0] = 0x%x\r\n", *buf);
					continue;
				}	

				msg = (ST_ZJCT_MNG_MSG *)buf;
				dev_idx = msg->ucData[1] - 1;			/* 0 ~ 31 */
				switch(msg->stHead.ucSrcAddr)
				{
					case ZJCT_DEV_ADDR_CY1H:
						index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;
						if(1 != gremote_mem_update[index].online)
						{
							continue;
						}
						break;
					case ZJCT_DEV_ADDR_DTJKH:
						if(1 != gremote_radio_update[dev_idx].online)
						{
							continue;
						}
						break;
					default:
						break;
		
				}

				if(( PANEL_ONLINE== panel_status)||((1 == g_dbg_panel)))
				{

					Zjct_AdptVoc_Msg_Process(buf, Rec_Len);

				}
			}
		}
	}
}


/*******************************************************************************
* Function:Zjct_AdptVoc_Socket_Close
*
* Description: �ر�ָ��ͨ�Ű��복ͨ���������л����շ���socket���
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/

static int32 Zjct_AdptVoc_Socket_Close(void)
{
	if (g_ZjctAdptVocSockFd)
	{
		close(g_ZjctAdptVocSockFd);
	}
	
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdptMng_Socket_init
*
* Description: ָ��ͨ�Ű��복ͨ������֮���������ͨ��Socket��ʼ��.
			��Socket�����շ���ͨϵͳ����Ĳ���������Ϣ��״̬��Ϣ ��
			Socket IP��ַ��ͨ��DHCP��ȡ�� ��ǰ�������ļ��ж�ȡ��
*
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	zjct��������ʱ���ù㲥��ַ��
*******************************************************************************/
static int32 Zjct_AdptMng_Socket_init(void)
{
	struct sockaddr_in my_addr = {0};

	int broadcast_flag = 1;

	/* ���չ㲥����*/
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(26112);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if ((g_ZjctAdptMngSockFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ERR(" create Zjct_AdptMng_Socket failed \r\n");
		return DRV_ERR;
	}

	if (DRV_OK != bind(g_ZjctAdptMngSockFd, (struct sockaddr *)&my_addr, sizeof(my_addr)))
	{
		ERR("Bind Zjct_AdptMng_Socket failed.\r\n");
		close(g_ZjctAdptMngSockFd);
		return DRV_ERR;
	}

	/* �����ܷ��ͥxx.xx.xx.255�Ĺ㲥����*/
	setsockopt(g_ZjctAdptMngSockFd, SOL_SOCKET, SO_BROADCAST, &broadcast_flag, sizeof(broadcast_flag));
	
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_AdptMng_RxThread
*
* Description: ָ��ͨ�Ű崦�����Գ�ͨ������(����UDP�˿ں�0x6600) ��
                    ϵͳ���������Ϣ��
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/

void Zjct_AdptMng_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	uint8 id = 0;
	uint8 process_flg = 1;
	
	LOG("Zjct_AdptMng_RxThread start \r\n");
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd, (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{

			process_flg = 1;
			
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				continue;
			}

			for(id = 0; id < CHETONG_SEAT_MAX; id++)
			{
				if(htonl(II_Adpt_local[id].ipaddr) == remote_addr.sin_addr.s_addr)
				{
					process_flg = 0;
					break;
				}
			}

			if(0 == process_flg)
			{
				continue;
			}
			
			if((glocal_mem_update[id].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 0, remote_addr.sin_addr.s_addr);

		}
	}
}

void Zjct_AdptMng_0_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[0], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[0].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[0].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 0, remote_addr.sin_addr.s_addr);

		}
	}
}

void Zjct_AdptMng_1_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[1], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[1].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[1].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 1, remote_addr.sin_addr.s_addr);

		}
	}
}

void Zjct_AdptMng_2_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[2], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[2].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[2].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 2, remote_addr.sin_addr.s_addr);

		}
	}
}


void Zjct_AdptMng_3_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[3], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[3].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[3].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 3, remote_addr.sin_addr.s_addr);

		}
	}
}


void Zjct_AdptMng_4_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[4], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[4].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[4].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 4, remote_addr.sin_addr.s_addr);

		}
	}
}


void Zjct_AdptMng_5_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[5], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[5].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[5].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 5, remote_addr.sin_addr.s_addr);

		}
	}
}


void Zjct_AdptMng_6_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[6], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[6].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[6].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 6, remote_addr.sin_addr.s_addr);

		}
	}
}


void Zjct_AdptMng_7_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[7], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[7].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[7].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 7, remote_addr.sin_addr.s_addr);

		}
	}
}


void Zjct_AdptMng_8_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[8], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[8].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[8].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 8, remote_addr.sin_addr.s_addr);

		}
	}
}


void Zjct_AdptMng_9_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[9], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[9].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[9].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 9, remote_addr.sin_addr.s_addr);

		}
	}
}


void Zjct_AdptMng_10_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[10], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[10].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[10].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 10, remote_addr.sin_addr.s_addr);

		}
	}
}


void Zjct_AdptMng_11_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 temp_buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int32 Rec_Len = 0;
	uint32 Len = 0;
	uint8 *buf = temp_buf;
	
	LOG("%s start \r\n", __func__);
	
	Len = sizeof(remote_addr);
	while (1)
	{
		memset(temp_buf, 0, sizeof(temp_buf));

		if ((Rec_Len = recvfrom(g_ZjctAdptMngSockFd_ID[11], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   	(struct sockaddr *)&remote_addr, &Len)) != DRV_ERR)
		{
			if(( PANEL_ONLINE != panel_status)&&((0 == g_dbg_panel)))
			{
				//continue;
			}

			if(htonl(II_Adpt_local[11].ipaddr) == remote_addr.sin_addr.s_addr)
			{
				continue;
			}
			
			if((glocal_mem_update[11].glocaldev_num == buf[6]) && (ZJCT_DEV_ADDR_CY1H == buf[2]))
			{
				Zjct_Terminal_Clash_SendTo_PC(buf[6]);
				ERR("have the same member in the system buf[6] = 0x%x\r\n", buf[6]);
				/*����Ϣ֪ͨPC��*/
				continue;
			}

			Zjct_AdptMng_Msg_Process(buf, Rec_Len, 11, remote_addr.sin_addr.s_addr);

		}
	}
}


/*******************************************************************************
* Function:Zjct_AdptMng_Socket_Close
*
* Description: �ر�ָ��ͨ�Ű��복ͨ����������ϵͳ�����շ���socket���
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/

static int32 Zjct_AdptMng_Socket_Close(void)
{
	if (g_ZjctAdptMngSockFd)
	{
		close(g_ZjctAdptMngSockFd);
	}
	
	return DRV_OK;
}

static int32 Zjct_AdptMng_Socket_init_ID(uint8 id)
{
	struct sockaddr_in my_addr = {0};

	int broadcast_flag = 1;

	/* ���չ㲥����*/
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(26114);
	my_addr.sin_addr.s_addr = htonl(II_Adpt_local[id].ipaddr);
	
	if ((g_ZjctAdptMngSockFd_ID[id] = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ERR(" create Zjct_AdptMng_Socket failed \r\n");
		return DRV_ERR;
	}

	if (DRV_OK != bind(g_ZjctAdptMngSockFd_ID[id], (struct sockaddr *)&my_addr, sizeof(my_addr)))
	{
		ERR("Bind Zjct_AdptMng_Socket failed.\r\n");
		close(g_ZjctAdptMngSockFd_ID[id]);
		return DRV_ERR;
	}

	/* �����ܷ��ͥxx.xx.xx.255�Ĺ㲥����*/
	setsockopt(g_ZjctAdptMngSockFd_ID[id], SOL_SOCKET, SO_BROADCAST, &broadcast_flag, sizeof(broadcast_flag));
	
	return DRV_OK;
}

static int32 Zjct_AdptVoc_Socket_init_ID(uint8 id)
{
	struct sockaddr_in my_addr = {0};

	int broadcast_flag = 1;

		/* ���չ㲥����*/
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(26115);
	my_addr.sin_addr.s_addr = htonl(II_Adpt_local[id].ipaddr);
	
	if ((g_ZjctAdptVocSockFd_ID[id] = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ERR("create Zjct_AdptVoc_Socket failed \r\n");
		return DRV_ERR;
	}

	if (DRV_OK != bind(g_ZjctAdptVocSockFd_ID[id], (struct sockaddr *)&my_addr, sizeof(my_addr)))
	{
		ERR("Bind Zjct_AdptVoc_Socket failed.\r\n");
		close(g_ZjctAdptVocSockFd_ID[id]);
		return DRV_ERR;
	}

	/* �����ܷ��ͥxx.xx.xx.255�Ĺ㲥����*/
	setsockopt(g_ZjctAdptVocSockFd_ID[id], SOL_SOCKET, SO_BROADCAST, &broadcast_flag, sizeof(broadcast_flag));
				
	return DRV_OK;
}
/*******************************************************************************
* Function: Zjct_IncCall_Socket_init
*
* Description: ָ��ͨ�Ű��복ͨ������(ʵ����INC)֮����в���
����ͨ��Socket��ʼ��.
��Socket�����շ���ͨ������Ϣ�����������շ� ��
Socket IP��ַ��ͨ��DHCP��ȡ�� ��ǰ�������ļ��ж�ȡ��
*
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/

static int32 Zjct_IncCall_Socket_init_ID(uint8 id)
{
	struct sockaddr_in my_addr;
	
	if (0 == II_Adpt_local[0].ipaddr)
	{
		ERR("Zjct_IncCall_Socket_init param isn't set\r\n");
		return DRV_ERR;
	}

	memset(&my_addr, 0x00, sizeof(my_addr));
	
	my_addr.sin_family = AF_INET;	//��������zjct����Ϣ
	my_addr.sin_port = htons(II_Inc_local[id].zjct_incsem_port);
	my_addr.sin_addr.s_addr = htonl(II_Inc_local[id].ipaddr);

	if ((g_ZjctIncCallSockFd_ID[id] = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ERR("zjct: can't create inc call socket.\r\n");
		return DRV_ERR;
	}

	if (DRV_ERR == bind(g_ZjctIncCallSockFd_ID[id], (struct sockaddr *)&my_addr, sizeof(my_addr)))
	{
		ERR("zjct: Bind inc call socket failed.\r\n");
		close(g_ZjctIncCallSockFd_ID[id]);
		return DRV_ERR;
	}
		
	return DRV_OK;
}

static int32 Zjct_IncCall_Sem_Process(uint8 *buf, int32 len, uint8 id)
{
	switch(buf[2])
	{
		case ZJCT_INC_VOICE_CMD:
			switch(buf[5])
			{
				case ZJCT_CALL_REQ:
					if((ZJCT_MODE_DH == glocal_mem_update[id].data.dev_mode)\
						||(ZJCT_MODE_QHFS == glocal_mem_update[id].data.dev_mode)\
						||(ZJCT_MODE_QHJS == glocal_mem_update[id].data.dev_mode)\
						||(ZJCT_MODE_CTZH == glocal_mem_update[id].data.dev_mode)\
						||(ZJCT_MODE_GTFS == glocal_mem_update[id].data.dev_mode)\
						||(ZJCT_MODE_BHFS == glocal_mem_update[id].data.dev_mode)\
						||(ZJCT_MODE_BHJS == glocal_mem_update[id].data.dev_mode)\
						||(ZJCT_MODE_DTFS == glocal_mem_update[id].data.dev_mode)\
						||(0 != glocal_mem_update[id].callIn)\
						||(0 != glocal_mem_update[id].inc_callIn))
					{				
						LOG("mode = %d, callIn = %d, inc_callIn = %d\r\n", glocal_mem_update[id].data.dev_mode,\
							glocal_mem_update[id].callIn, glocal_mem_update[id].inc_callIn);
							
						//BoardSem_CallAck_busy(buf, len);
						return DRV_OK;
					}
					
					Buzzer_Control(FMQ_CTRL_SLOW);
					buzzer_status = FMQ_CTRL_SLOW;
					glocal_mem_update[id].inc_callIn = 1;
					
					DBG("seat receive a callin\r\n");			
					break;
				case ZJCT_CALL_ACK:

					DBG("seat receive a callout ack\r\n");							
					break;
				case ZJCT_CONNECT_REQ:
					glocal_mem_update[id].inc_connect = ZJCT_INC_CONNECT;
					DBG("seat receive a callout connect req \r\n");
					break;
				case ZJCT_RELEASE_REQ:
					if(ZJCT_MODE_QHJS != glocal_mem_update[id].data.dev_mode)
					{
						if(FMQ_CTRL_OFF != buzzer_status)
						{
							Buzzer_Control(FMQ_CTRL_OFF);
							buzzer_status = FMQ_CTRL_OFF;
						}
						glocal_mem_update[id].inc_connect = ZJCT_INC_DISCONNECT;
						glocal_mem_update[id].inc_callIn = 0;
						Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update[id].data, \
								ZJCT_STATUS_MSG_DATA_LEN);
						
						DBG("seat receive release req\r\n");
					}
					else
					{
						return DRV_OK;
					}
					break;					
				case ZJCT_RELEASE_ACK:
					if(ZJCT_MODE_QHJS == glocal_mem_update[id].data.dev_mode)
					{
						return DRV_OK;
					}
					
					DBG("seat receive release ack\r\n");
					break;
				default: 
					break;
			}

			Zjct_SeatMng_Socket_Send(buf, len);
			
			DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", len);
			break;
		default:
			ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
			break;
	}
	
	return DRV_OK;
}

void Zjct_IncCall_0_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[0], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 0);
		}
	}
}

void Zjct_IncCall_1_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[1], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 1);
		}
	}
}

void Zjct_IncCall_2_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[2], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 2);
		}
	}
}

void Zjct_IncCall_3_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[3], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 3);
		}
	}
}


void Zjct_IncCall_4_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[4], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 4);
		}
	}
}

void Zjct_IncCall_5_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[5], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 5);
		}
	}
}


void Zjct_IncCall_6_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[6], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 6);
		}
	}
}

void Zjct_IncCall_7_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[7], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 7);
		}
	}
}


void Zjct_IncCall_8_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[8], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 8);
		}
	}
}

void Zjct_IncCall_9_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[9], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 9);
		}
	}
}

void Zjct_IncCall_10_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[10], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 10);
		}
	}
}


void Zjct_IncCall_11_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	len = sizeof(remote_addr);
	
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncCallSockFd_ID[11], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
			Zjct_IncCall_Sem_Process(buf, Rec_Len, 11);
		}
	}
}


/*******************************************************************************
* Function: Zjct_IncCall_Socket_Close
*
* Description: g_ZjctIncCallSockFd ����ر�
*
* Inputs: ��
*	   
* Outputs: ��
*		
* Returns: ��
*
* Comments: 	
*******************************************************************************/
static int32 Zjct_IncCall_Socket_Close(uint8 id)
{	
	if(g_ZjctIncCallSockFd_ID[id])
	{
		close(g_ZjctIncCallSockFd_ID[id]);
	}

	return DRV_OK;
}

static int32 Zjct_IncCall_VocData_Process(uint8 *buf, uint32 len, uint8 id)
{
	if ((id > 12)||(NULL == buf) ||(ZJCT_INC_VOC_20MS_LEN_166 != len ))
	{
		ERR("Zjct_IncCall_VocData_Process buf or len error"
			"buf = 0x%x, len = %d\r\n", buf, len);
		return DRV_ERR;
	}

	Zjct_Conference_Mainboard_To_Ac491(AC491_VOC_CHAN_FOR_DIAL, buf + 6, \
		ZJCT_VOC_G711_20MS_LEN_160);

	return DRV_OK;	
}

static int32 Zjct_IncVoc_Socket_init_ID(uint8 id)
{
	struct sockaddr_in my_addr = {0};
	
	memset(&my_addr, 0x00, sizeof(my_addr));
	
	my_addr.sin_family = AF_INET;	//��������zjct����Ϣ
	my_addr.sin_port = htons(II_Inc_local[id].zjct_incvoc_port);
	my_addr.sin_addr.s_addr = htonl(II_Inc_local[id].ipaddr);

	if ((g_ZjctIncVocSockFd_ID[id] = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ERR("create Zjct_IncVoc_Socket failed \r\n");
		
		return DRV_ERR;
	}

	if (DRV_ERR == bind(g_ZjctIncVocSockFd_ID[id], (struct sockaddr *)&my_addr, sizeof(my_addr)))
	{
		ERR("Bind Zjct_IncVoc_Socket failed.\r\n");
		close(g_ZjctIncVocSockFd_ID[id]);
		
		return DRV_ERR;
	}
		
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_IncVoc_RxThread
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:  ��������ʱ���þ��g_ZjctIncVocSockFd  ����������
*******************************************************************************/
void Zjct_IncVoc_0_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[0], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[0].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[0].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 0);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_1_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[1], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[1].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[1].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 1);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_2_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[2], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[2].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[2].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 2);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_3_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[3], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[3].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[3].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 3);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_4_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[4], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[4].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[4].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 4);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_5_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[5], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[5].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[5].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 5);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_6_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[6], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[6].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[6].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 6);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_7_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[7], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[7].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[7].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 7);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_8_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[8], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[8].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[8].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 8);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}


void Zjct_IncVoc_9_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[9], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[9].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[9].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 9);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_10_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[10], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[10].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[10].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 10);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

void Zjct_IncVoc_11_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600] = {0};
	int Rec_Len;
	uint32 len;
	
	LOG("%s start \r\n", __func__);
	
	len = sizeof(remote_addr);
	while (1)
	{
		if((Rec_Len = recvfrom(g_ZjctIncVocSockFd_ID[11], (void *)buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{
		
			//Zjct_IncVoc_Sendto_Ac491_Process(buf, Rec_Len);	

			switch(buf[2])
			{
/*
				case ZJCT_INC_VOICE_CMD:
					DBG("Zjct_IncCall_RxThread receive one msg Rec_Len = %d\r\n", Rec_Len);
					Zjct_SeatCall_Socket_Send(buf, Rec_Len);
					break;
*/					
				case ZJCT_INC_VOICE_DATA:
					if((glocal_mem_update[11].glocaldev_num == buf[5])&&(ZJCT_INC_CONNECT == glocal_mem_update[11].inc_connect))
					{
						static uint8 i = 0;
						
						Zjct_IncCall_VocData_Process(buf, Rec_Len, 11);

						i++;
						if(200 == i)
						{
							i = 0;
#if 0
							uint8 j = 0;
							printf("111%d:::", len);
		
							for(j = 0; j < 10; j++)
							{
								printf("0x%x:", buf[j]);
							}
							
							putchar('\n');
#endif
						}
					}
					else
					{
						continue;
					}		
					break;				
				default:
					ERR("Zjct_IncCall_RxThread Param error buf[2] = 0x%x\r\n", buf[2]);
					break;
			}

		}
	}
}

/*******************************************************************************
* Function: Zjct_IncVoc_Socket_Close
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/

static int32 Zjct_IncVoc_Socket_Close(uint8 id)
{	
	if  (g_ZjctIncVocSockFd_ID[id])
	{
		close(g_ZjctIncVocSockFd_ID[id]);
	}
	
	return DRV_OK;
}


/*******************************************************************************
* Function: Zjct_SeatStatusMsg_Process
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments: 
		
*******************************************************************************/


int32 Zjct_DanHu_Status_TimeOut_Check(void)
{
	if((time_cnt_100ms - danhu_cnt) > PERIOD_15S)
	{
		ST_ZJCT_MNG_MSG send_msg;

		memset(&send_msg, 0, sizeof(send_msg));

		gdanhu_timeout_flg = 0;
		glocal_mem_update[0].callIn = 0;
		glocal_mem_update[0].connect = 0;	
       	glocal_mem_update[0].callIn_id = 0;	

		send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
		send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
		send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
		send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;

		send_msg.ucData[3] = ZJCT_MODE_DH;
		send_msg.ucData[DANHU_MSG_BYTE] = DANHU_TIMEOUT;
		
		Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);

		DBG("danhu timeout error\r\n");

	}
	
	return DRV_OK;
}

int32 Zjct_Handle_Status_Check(void)
{
	if(0 == first_cnt)
	{
		first_cnt = 1;
		return DRV_OK;
	}
	
	if((PANEL_ONLINE == panel_status)&&\
		((time_cnt_100ms - handle_time_cnt) > PERIOD_3S))
	{
		panel_status = PANEL_OFFLINE;

		glocal_mem_update[0].connect = 0;
     	glocal_mem_update[0].callIn_id = 0;		
		
		glocal_mem_update[0].callIn = 0;
		glocal_mem_update[0].force_call_srcId =0;
		glocal_mem_update[0].radio_ctl_status = 0;
		glocal_mem_update[0].radio_gtctl_status = 0;
		glocal_mem_update[0].inc_callIn = 0;
		glocal_mem_update[0].inc_connect = 0;
		glocal_mem_update[0].group_call_status = 0;
		glocal_mem_update[0].group_call_id = 0;		
	
		glocal_mem_update[0].data.dev_mode = ZJCT_MODE_CNGB;
		glocal_mem_update[0].data.dev_dscId = ZJCT_BROADCASE_ADDR;
		
		DBG(" operate panel offline time_cnt_100ms= %d, "
			"handle_time_cnt = %d\r\n", time_cnt_100ms, handle_time_cnt);
	}

	return DRV_OK;
}

int32 Zjct_Handle_Status_Frame_SendTo_PC(void)
{
	ST_ZJCT_MNG_MSG send_msg;
	
	memset(&send_msg, 0, sizeof(send_msg));
	
	send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
	send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
	send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;

	send_msg.ucData[1] = glocal_mem_update[0].data.dev_mode;
	send_msg.ucData[2] = glocal_mem_update[0].glocaldev_num;
	send_msg.ucData[3] = ZJCT_MODE_HANDLE;
	memcpy(&send_msg.ucData[4], \
		&glocal_mem_update[0].data.dev_dscId, 2);

	send_msg.ucData[DANHU_MSG_BYTE - 1] = gwork_mode;		
	send_msg.ucData[DANHU_MSG_BYTE] = HANDLE_PC;
	
	send_msg.ucData[DANHU_MSG_BYTE + 1] = glocal_mem_update[0].data.user_num[0];
	send_msg.ucData[DANHU_MSG_BYTE + 2] = glocal_mem_update[0].data.user_num[1];
	memcpy(&send_msg.ucData[DANHU_MSG_BYTE + 3], \
			&glocal_mem_update[0].data.user_num[2], send_msg.ucData[DANHU_MSG_BYTE + 2]);
	
	memcpy(&send_msg.ucData[24], &glocal_mem_update[0].Ipaddr,  4);
	memcpy(&send_msg.ucData[28], &glocal_mem_update[0].Netmask, 1);

	Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);

	return DRV_OK;
}

static int32 Zjct_Terminal_Clash_SendTo_PC(uint8 id)
{
	ST_ZJCT_MNG_MSG send_msg;
	
	memset(&send_msg, 0, sizeof(send_msg));
	
	send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
	send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
	send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;

	send_msg.ucData[2] = glocal_mem_update[id].glocaldev_num;
	send_msg.ucData[3] = ZJCT_MODE_TERMINAL_CLASH;
	memcpy(&send_msg.ucData[4], \
		&glocal_mem_update[id].data.dev_dscId, 2);

	send_msg.ucData[DANHU_MSG_BYTE - 1] = gwork_mode;		
	send_msg.ucData[DANHU_MSG_BYTE] = HANDLE_PC;
	
	send_msg.ucData[DANHU_MSG_BYTE + 1] = glocal_mem_update[id].data.user_num[0];
	send_msg.ucData[DANHU_MSG_BYTE + 2] = glocal_mem_update[id].data.user_num[1];
	memcpy(&send_msg.ucData[DANHU_MSG_BYTE + 3], \
			&glocal_mem_update[id].data.user_num[2], send_msg.ucData[DANHU_MSG_BYTE + 2]);
	
	memcpy(&send_msg.ucData[24], &glocal_mem_update[id].Ipaddr,  4);
	memcpy(&send_msg.ucData[28], &glocal_mem_update[id].Netmask, 1);
	
	Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);

	return DRV_OK;
}


static int32 Zjct_SeatMng_Status_Inquire_terminal(uint8 *buf, uint32 len)
{
	if ((NULL == buf) ||(0x00 == len ))
	{
		ERR("%s buf or len error buf = 0x%x, len = %d\r\n",
				__func__, buf, len);
		return DRV_ERR;
	}

	uint8 temp_cnt =0;
	
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;
	ST_ZJCT_MNG_MSG send_msg;

	int16 dev_type = 0, dev_idx = 0, index =0;

	memset(&send_msg, 0, sizeof(send_msg));

	switch(msg->ucData[DANHU_MSG_BYTE])
	{
		case INQUIRE_ALL_TERMINAL:

			send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
			send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
			send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
			send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;

			send_msg.ucData[3] = ZJCT_MODE_INQUIRE_MEM;
			send_msg.ucData[4] = ZJCT_DEV_ADDR_CY1H;
			
			send_msg.ucData[DANHU_MSG_BYTE] = INQUIRE_ALL_TERMINAL_ACK;
			
			temp_cnt = online_mem_cnt;
			send_msg.ucData[DANHU_MSG_BYTE + 1] = temp_cnt;
			
			memcpy(&send_msg.ucData[DANHU_MSG_BYTE + 2], \
					(uint8 *)&online_mem, 4);

			Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);

			break;
		case INQUIRE_ONE_TERMINAL:

			if(msg->ucData[1] > ZJCT_DEV_INDX_MAX)
			{
				ERR("msg->ucData[1] value error  0x%x\r\n", msg->ucData[1]);
				
				return DRV_ERR;
			}
			dev_type = CY1HH_TYPE_IDX;
			dev_idx = msg->ucData[1] - 1;			/* 0 ~ 31 */
			index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;
			
			send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
			send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
			send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
			send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
			
			if( glocal_mem_update[0].glocaldev_num != msg->ucData[1])
			{
				send_msg.ucData[1] = msg->ucData[1];
				send_msg.ucData[3] = ZJCT_MODE_INQUIRE_MEM;
				memcpy(&send_msg.ucData[4], \
						&gremote_mem_update[index].data.dev_dscId, 2);

				memcpy(&send_msg.ucData[6], &gremote_mem_update[index].data.hw_ver, 2);
				memcpy(&send_msg.ucData[8], &gremote_mem_update[index].data.sw_ver, 4);

				send_msg.ucData[DANHU_MSG_BYTE - 1] = gremote_mem_update[index].data.dev_mode;
				send_msg.ucData[DANHU_MSG_BYTE] = INQUIRE_ONE_TERMINAL_ACK;

				memcpy(&send_msg.ucData[24], &gremote_mem_update[index].Ipaddr,  4);
				memcpy(&send_msg.ucData[28], &gremote_mem_update[index].Netmask, 1);

				if((1 == gremote_mem_update[index].data.user_num[0])
					&&(gremote_mem_update[index].data.user_num[1] < 10))
				{
					send_msg.ucData[DANHU_MSG_BYTE + 1] = gremote_mem_update[index].data.user_num[0];
					send_msg.ucData[DANHU_MSG_BYTE + 2] = gremote_mem_update[index].data.user_num[1];
					memcpy(&send_msg.ucData[DANHU_MSG_BYTE + 3], \
						&gremote_mem_update[index].data.user_num[2], send_msg.ucData[DANHU_MSG_BYTE + 2]);
				}
				else
				{
					send_msg.ucData[DANHU_MSG_BYTE + 1] = 0;
					send_msg.ucData[DANHU_MSG_BYTE + 2] = 0;
				}
				
			}
			else
			{
				send_msg.ucData[1] = glocal_mem_update[0].glocaldev_num;
				send_msg.ucData[3] = ZJCT_MODE_INQUIRE_MEM;
				memcpy(&send_msg.ucData[4], \
						&glocal_mem_update[0].data.dev_dscId, 2);
			
				memcpy(&send_msg.ucData[6], &glocal_mem_update[0].data.hw_ver, 2);
				memcpy(&send_msg.ucData[8], &glocal_mem_update[0].data.sw_ver, 4);

				send_msg.ucData[DANHU_MSG_BYTE - 1] = glocal_mem_update[0].data.dev_mode;;
				send_msg.ucData[DANHU_MSG_BYTE] = INQUIRE_ONE_TERMINAL_ACK;

				send_msg.ucData[DANHU_MSG_BYTE + 1] = glocal_mem_update[0].data.user_num[0];
				send_msg.ucData[DANHU_MSG_BYTE + 2] = glocal_mem_update[0].data.user_num[1];
				memcpy(&send_msg.ucData[DANHU_MSG_BYTE + 3], \
						&glocal_mem_update[0].data.user_num[2], send_msg.ucData[DANHU_MSG_BYTE + 2]);				

				memcpy(&send_msg.ucData[24], &glocal_mem_update[0].Ipaddr,  4);
				memcpy(&send_msg.ucData[28], &glocal_mem_update[0].Netmask, 1);
				
			}

			Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);
			
			break;
		default:
			break;
	}
	
	return DRV_OK;
}

static int32 Zjct_SeatMng_Status_Inquire_radio(uint8 *buf, uint32 len)
{
	if ((NULL == buf) ||(0x00 == len ))
	{
		ERR("%s buf or len error buf = 0x%x, len = %d\r\n",
				__func__, buf, len);
		return DRV_ERR;
	}

	uint8 temp_cnt =0;
	int16 dev_idx = 0;
	
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;
	ST_ZJCT_MNG_MSG send_msg;
	
	memset(&send_msg, 0, sizeof(send_msg));

	switch(msg->ucData[DANHU_MSG_BYTE])
	{
		case INQUIRE_ALL_RADIO:
			send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
			send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
			send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
			send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;

			send_msg.ucData[3] = ZJCT_MODE_INQUIRE_RADIO;
			send_msg.ucData[DANHU_MSG_BYTE] = INQUIRE_ALL_RADIO_ACK;
					
			temp_cnt = online_radio_cnt;
			send_msg.ucData[DANHU_MSG_BYTE + 1] = temp_cnt;
			
			memcpy(&send_msg.ucData[DANHU_MSG_BYTE + 2], \
					(uint8 *)&online_radio, 4);

			Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);

			break;
		case INQUIRE_ONE_RADIO:
			if(msg->ucData[1] > RADIO_NUM_MAX)
			{
				ERR("msg->ucData[1] value error  0x%x\r\n", msg->ucData[1]);
				
				return DRV_ERR;
			}

			dev_idx = msg->ucData[1] - 1;	
			
			send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
			send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
			send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
			send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;
			
			send_msg.ucData[1] = msg->ucData[1];
			send_msg.ucData[3] = ZJCT_MODE_INQUIRE_RADIO;

			memcpy(&send_msg.ucData[4], \
				&gremote_radio_update[dev_idx].mem_info.dev_dscId, 2);

			memcpy(&send_msg.ucData[6], &gremote_radio_update[dev_idx].hw_ver, 2);
			memcpy(&send_msg.ucData[8], &gremote_radio_update[dev_idx].sw_ver, 4);
			
			send_msg.ucData[DANHU_MSG_BYTE - 1] = gremote_radio_update[dev_idx].mem_info.dev_mode;
			send_msg.ucData[DANHU_MSG_BYTE] = INQUIRE_ONE_RADIO_ACK;

			memcpy(&send_msg.ucData[24], &gremote_radio_update[dev_idx].ip_addr,  4);
			memcpy(&send_msg.ucData[28], &gremote_radio_update[dev_idx].net_mask, 1);

			Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);

			break;
		default:
			break;
	}
	
	return DRV_OK;
}

static int32 Zjct_SeatStatusMsg_Process(uint8 *buf, uint32 len)
{
	if ((NULL == buf) ||(0x00 == len ))
	{
		ERR("Zjct_SeatStatusMsg_Process buf or len error"
			"buf = 0x%x, len = %d\r\n", buf, len);
		return DRV_ERR;
	}
	
	ST_ZJCT_MNG_MSG Msg, *msg = NULL;
	//char var_val[10] = {0};
	
	memset(&Msg, 0, sizeof(Msg));
	memcpy(&Msg, buf, len);

	msg = &Msg;

	ST_ZJCT_MNG_MSG send_msg;

	memset(&send_msg, 0, sizeof(send_msg));
	uint16 dev_type = 0, dev_idx = 0, index = 0, i = 0;
	uint8 reset_flg = 0;

	switch(msg->stHead.ucSrcAddr)
	{
		case ZJCT_DEV_ADDR_ZHZXH:
			/*�������Ŀ��ƺе�״̬��ѯ��־*/
			//DBG("Center ctrl box \r\n");
			break;
		case ZJCT_DEV_ADDR_DTJKH:
			/* ��̨�ն�1~12��Ӧ���ն˵�ַΪ0x1~0xC*/
			break;
		case ZJCT_DEV_ADDR_CY1H:
			/* II���ն�1~32��Ӧ���ն˵�ַΪ0x1~0x20 */
			switch(msg->ucData[3])
			{
				case ZJCT_MODE_QHFS:
					/* ǿ������*/
					if(FORCECALLOUT == msg->ucData[DANHU_MSG_BYTE])
					{	
						/* ����ǿ������*/
						Zjct_Local_Mem_Data_Push((uint8 *)&glocal_mem_update[0].data, 
									ZJCT_STATUS_MSG_DATA_LEN);

						glocal_mem_update[0].data.dev_mode = ZJCT_MODE_QHFS;
						glocal_mem_update[0].data.dev_dscId = *((uint16 *)&(msg->ucData[0x04]));
						DBG("force call out\r\n");
					}
					else if(CANCEL_FORCECALLOUT == msg->ucData[DANHU_MSG_BYTE])
					{
						/* ����ȡ��ǿ������*/
						Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update[0].data, 
									ZJCT_STATUS_MSG_DATA_LEN);

						DBG("cancel one force call out \r\n");
					}
					break;
				case ZJCT_MODE_QHJS:
					/* ǿ������*/ //���������Adptmng����			
					break;
				case ZJCT_MODE_CTZH:
					/* ��ͨ���*/
					dev_type = CY1HH_TYPE_IDX;

					if(GROUPCALLOUT == msg->ucData[DANHU_MSG_BYTE])
					{
						Zjct_Local_Mem_Data_Push((uint8 *)&glocal_mem_update[0].data,\
												ZJCT_STATUS_MSG_DATA_LEN);

						glocal_mem_update[0].data.dev_mode = ZJCT_MODE_CTZH;
						//glocal_mem_update.data.dev_dscId = CY1HH_F400 | glocal_mem_update.glocaldev_num ;
						glocal_mem_update[0].data.dev_dscId = *((uint16 *)&(msg->ucData[0x04]));

						gauto_send_cfg_flg = 1;	
						g_groudcall_cnt = 0;
						
						memcpy(groupcall_mem, &msg->ucData[DANHU_MSG_BYTE + 1], \
									msg->ucData[DANHU_MSG_BYTE + 1] + 1);

						DBG("send a group call out\r\n");

					}
					else if(CANCEL_GROUPCALLOUT == msg->ucData[DANHU_MSG_BYTE])
					{
						gauto_send_cfg_flg = 0;	
						
						Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update[0].data,\
											ZJCT_STATUS_MSG_DATA_LEN);

						Zjct_AdptMng_AutoSend_Close_GroupCall();

						for (i = 0; i < groupcall_mem[0]; i++)
						{
							dev_idx = groupcall_mem[i + 1] - 1 ;
							index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

							gremote_mem_update[index].group_call_status = ZJCT_DISCONNECT;
							DBG("send Member %d leave the group\r\n",  groupcall_mem[i + 1]);
							
						}
						
						groupcall_mem[0] = 0;

						DBG("cancel a group call out\r\n");
					}	
					else if(GROUPCALL_HANDUP == msg->ucData[DANHU_MSG_BYTE])
					{
						glocal_mem_update[0].data.dev_mode = ZJCT_MODE_CNGB;
						glocal_mem_update[0].data.dev_dscId  = ZJCT_BROADCASE_ADDR;
					}
					break;
				case ZJCT_MODE_DH:
					/* ����*/
					switch(msg->ucData[DANHU_MSG_BYTE])
					{
						case DANHU_ANSWER: 		/* 0x02 ����adapter ����Ӧ��*/

							Zjct_Local_Mem_Data_Push((uint8 *)&glocal_mem_update[0].data, \
												ZJCT_STATUS_MSG_DATA_LEN);

							glocal_mem_update[0].data.dev_mode = ZJCT_MODE_DH;
							glocal_mem_update[0].data.dev_dscId = *((uint16 *)&(msg->ucData[0x04]));
							glocal_mem_update[0].connect = 1;
							
							//Buzzer_Control(FMQ_CTRL_OFF);
							if(FMQ_CTRL_OFF != buzzer_status)
							{
								Buzzer_Control(FMQ_CTRL_OFF);
								buzzer_status = FMQ_CTRL_OFF;
							}
							DBG("ack the phone call in\r\n");
							break;
						case DANHU_CALLOUT:		/*0x03 Seat ���к���*/
							dev_type = CY1HH_TYPE_IDX;
							dev_idx = msg->ucData[5] - 1;			/* 0 ~ 31 */
							index = ZJCT_DEV_INDX_MAX * dev_type + dev_idx;

							if((ZJCT_MODE_DH == gremote_mem_save[index].data.dev_mode)\
								||(ZJCT_MODE_QHFS == gremote_mem_save[index].data.dev_mode)\
								||(ZJCT_MODE_QHJS == gremote_mem_save[index].data.dev_mode)\
								||(ZJCT_MODE_CTZH == gremote_mem_save[index].data.dev_mode)\
								||(ZJCT_MODE_GTFS == gremote_mem_save[index].data.dev_mode)\
								||(ZJCT_MODE_BHFS == gremote_mem_save[index].data.dev_mode)\
								||(ZJCT_MODE_BHJS == gremote_mem_save[index].data.dev_mode)\
								||(ZJCT_MODE_DTFS == gremote_mem_save[index].data.dev_mode))						
							{
								send_msg.stHead.ucType = ZJCT_SEAT_TXB_CMD;
								send_msg.stHead.ucDstAddr = ZJCT_DEV_ADDR_BRDCAST;
								send_msg.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
								send_msg.stHead.usLen = ZJCT_STATUS_MSG_DATA_LEN;

								send_msg.ucData[3] = ZJCT_MODE_DH;
								send_msg.ucData[4] = msg->ucData[4];
								send_msg.ucData[5] = msg->ucData[5];

								send_msg.ucData[DANHU_MSG_BYTE] = DANHU_BUSY;

								Zjct_SeatMng_Socket_Send((uint8 *)&send_msg, ZJCT_STATUS_MSG_LEN);

								DBG("the phone you call busy");
								break;
							}

							danhu_cnt = time_cnt_100ms;
							gdanhu_timeout_flg = 1;

							Zjct_Local_Mem_Data_Push((uint8 *)&glocal_mem_update[0].data, \
								ZJCT_STATUS_MSG_DATA_LEN);
							
							glocal_mem_update[0].callIn = 2;
							glocal_mem_update[0].connect = 0;
							glocal_mem_update[0].callIn_id = msg->ucData[0x05];	
							glocal_mem_update[0].data.dev_dscId = *(uint16 *)(&msg->ucData[0x04]);
							glocal_mem_update[0].data.dev_mode = ZJCT_MODE_DH;
							
							DBG("the phone call out \r\n");
							break;
						case DANHU_DISCONNECT_PASSIVE_ACK: /* 0x06 �����Ҷ�Ӧ��*/
							
							/*
							Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update.data, \
								ZJCT_STATUS_MSG_DATA_LEN);

							glocal_mem_update.callIn = 0;
							glocal_mem_update.connect = 0;
							DBG("ack auto hangup \r\n");
							*/
							break;
						case DANHU_DISCONNECT_ACTIVE:		/* 0x07 �����Ҷϵ绰*/
							Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update[0].data, \
								ZJCT_STATUS_MSG_DATA_LEN);

							//glocal_mem_update.callIn = 0;
							//glocal_mem_update.connect = 0;
							if(1 == glocal_mem_update[0].connect)
							{
								glocal_mem_update[0].active_release = 1;
							}
							
							//Buzzer_Control(FMQ_CTRL_OFF);
							if(1 == gdanhu_timeout_flg)
							{
								gdanhu_timeout_flg = 0;
							}
							
							DBG("need hangup\r\n");
							break;
						case DANHU_CALLIN_REFUSE:
							//Buzzer_Control(FMQ_CTRL_OFF);
							if(FMQ_CTRL_OFF != buzzer_status)
							{
								Buzzer_Control(FMQ_CTRL_OFF);
								buzzer_status = FMQ_CTRL_OFF;
							}
							break;			
						default:
							ERR("case ZJCT_MODE_DH param error! msg->ucData[DANHU_MSG_BYTE] = 0x%x\r\n",\
								msg->ucData[DANHU_MSG_BYTE]);
							break;
					}
					
					break;	
				case ZJCT_MODE_CNGB:
					/* ���ڹ㲥*/
					glocal_mem_update[0].data.dev_mode = ZJCT_MODE_CNGB;
					glocal_mem_update[0].data.dev_dscId  = ZJCT_BROADCASE_ADDR;
					break;	
				case ZJCT_MODE_KZDTFS:
					/* ���Ƶ�̨����*/
					/* PTT ���µ�ʱ���ɽ���ת�ɷ���*/
					//glocal_mem_update.data.dev_mode = ZJCT_MODE_KZDTFS;
					//glocal_mem_update.data.dev_dscId = *((uint16 *)&(msg->ucData[0x04]));
					break;
				case ZJCT_MODE_KZDTJS:
					/* ���Ƶ�̨����*/
					glocal_mem_update[0].data.dev_mode = ZJCT_MODE_KZDTJS;
					glocal_mem_update[0].data.dev_dscId = *((uint16 *)&(msg->ucData[0x04]));
					DBG("ZJCT_MODE_KZDTJS Control the radio %d receive voice \r\n", msg->ucData[0x05]);
					break;
				case ZJCT_MODE_GTFS:
					/* ��������*/
					/* PTT ���µ�ʱ���ɽ���ת�ɷ���*/
					//glocal_mem_update.data.dev_mode = ZJCT_MODE_GTFS;
					//glocal_mem_update.data.dev_dscId = *((uint16 *)&(msg->ucData[0x04]));
					break;
				case ZJCT_MODE_GTJS:
					/* ��������*/
					glocal_mem_update[0].data.dev_mode = ZJCT_MODE_GTJS;
					glocal_mem_update[0].data.dev_dscId = *((uint16 *)&(msg->ucData[0x04]));
					DBG("ZJCT_MODE_GTJS Control the radio receive voice \r\n");
					break;
				case ZJCT_MODE_BHFS:
					/* ���Ž�������*/			
					break;
				case ZJCT_MODE_BHJS:
					/* ���Ž�������*/
					break;
				case ZJCT_MODE_SETTING:
					switch(msg->ucData[DANHU_MSG_BYTE])
					{
						case ADDR_SETTING:
							if(msg->ucData[DANHU_MSG_BYTE + 1] < 32)
							{
								Zjct_Clean_Bit((uint8 *)&online_mem, glocal_mem_update[0].glocaldev_num - 1);
								
								glocal_mem_update[0].data.user_num[0] = 0;
								 
								memset(&glocal_mem_update[0].data.user_num[2], \
									0x00, glocal_mem_update[0].data.user_num[1]);
									
								glocal_mem_update[0].glocaldev_num = msg->ucData[DANHU_MSG_BYTE + 1];

								change_config_cat_var_val(config_cfg, ETH, "localdev_num", msg->ucData[DANHU_MSG_BYTE + 1]);

								rewrite_config(config_cfg);
								
								Zjct_AdptMng_SendParamConfig_Req(glocal_mem_update[0].glocaldev_num);	
								
								Zjct_Set_Bit((uint8 *)&online_mem, glocal_mem_update[0].glocaldev_num - 1);
							}
							break;
						default:
							ERR("ZJCT_MODE_SETTING mode error msg->ucData[DANHU_MSG_BYTE] "
								"= 0x%x\r\n",  msg->ucData[DANHU_MSG_BYTE]);
							break;
					}
					break;
				case ZJCT_MODE_INQUIRE_MEM:
					Zjct_SeatMng_Status_Inquire_terminal(buf, len);
					break;
				case ZJCT_MODE_INQUIRE_RADIO:
					Zjct_SeatMng_Status_Inquire_radio(buf, len);
					break;
				case ZJCT_MODE_HANDLE:
					
					Zjct_Handle_Status_Frame_SendTo_PC();
					
					switch(msg->ucData[DANHU_MSG_BYTE])
					{
						case HANDLE_INIT:
							
							break;
						case HANDLE_PC:
							handle_time_cnt = time_cnt_100ms;
						
							if((PANEL_OFFLINE == panel_status))
							{
								panel_status = PANEL_ONLINE;
								
								DBG("operate panel online \r\n");

								glocal_mem_update[0].connect = 0;
								glocal_mem_update[0].callIn_id = 0;
								glocal_mem_update[0].callIn = 0;
								glocal_mem_update[0].active_release = 0;
								glocal_mem_update[0].force_call_srcId =0;
								glocal_mem_update[0].radio_ctl_status = 0;
								glocal_mem_update[0].radio_gtctl_status = 0;
								glocal_mem_update[0].inc_callIn = 0;
								glocal_mem_update[0].inc_connect = 0;
								glocal_mem_update[0].group_call_status = 0;
								glocal_mem_update[0].group_call_id = 0;		
							
								glocal_mem_update[0].data.dev_mode = ZJCT_MODE_CNGB;
								glocal_mem_update[0].data.dev_dscId = ZJCT_BROADCASE_ADDR;
								gauto_send_cfg_flg = 0;
									
								DBG("panel_status = %d, handle_time_cnt = %d,"
										" time_cnt_100ms = %d\r\n" ,panel_status, handle_time_cnt, time_cnt_100ms);
							
							}

							break;
						default:
							ERR("ZJCT_MODE_SETTING mode error msg->ucData[DANHU_MSG_BYTE] "
								"= 0x%x\r\n",  msg->ucData[DANHU_MSG_BYTE]);
							break;
					}
				
					break;
				case ZJCT_MODE_WORKMODE:
					switch(msg->ucData[DANHU_MSG_BYTE])
					{
						case SEAT_MODE:
							
							if (SEAT_MODE != gwork_mode)
							{
								//change_config_cat_var_val(config_cfg, INFO, "mode", SEAT_MODE);
								//rewrite_config(config_cfg);
								
								reset_flg  = 1;
								LOG("switch to haifang mode \r\n");
							}
							else
							{
								LOG(" SEAT_MODE the workmode need not change\r\n");
							}
							break;
						case CY1HH_MODE:
							if (CY1HH_MODE != gwork_mode)
							{
								//change_config_cat_var_val(mgw_cfg, INFO, "mode", CY1HH_MODE);

								reset_flg = 1;
								LOG("switch to ZJCT mode \r\n");
							}
							else
							{
								LOG(" CY1HH_MODE the workmode need not change\r\n");
							}
							break;
						default:
							ERR("ZJCT_MODE_WORKMODE mode error msg->ucData[DANHU_MSG_BYTE] "
								"= 0x%x\r\n",  msg->ucData[DANHU_MSG_BYTE]);
							break;
					}
					
					if (1 == reset_flg)
					{
						sleep(1);
						//system_reboot();
					}
					break;
				default:
					ERR(" case ZJCT_DEV_ADDR_CY1H param error! msg->ucData[3] = 0x%x\r\n",\
								msg->ucData[3]);
					break;		
			}
			
			break;
		case ZJCT_DEV_ADDR_CY2H:
			/* ��Ա2�ź�*/
			break;
		case ZJCT_DEV_ADDR_YHJKH:
			/* �û��ӿں� */
			break;
		case ZJCT_DEV_ADDR_YHKZH:
			/* �û���չ�� */
			break;
		case ZJCT_DEV_ADDR_ZY1H:
			/* ��Ա1�ź�*/
			break;
		default:
			ERR("Zjct_SeatStatusMsg_Process param error! msg->stHead.ucSrcAddr = 0x%x\r\n",\
									msg->stHead.ucSrcAddr);
			break;
	}

	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_SeatRadioMsg_Process
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments: 
		
*******************************************************************************/
static int32 Zjct_zhuanxin_stop(void)
{
	if(DTZX_STOP == radio_ZX_status)
	{
		return DRV_OK;
	}
	
	ST_ZJCT_MNG_MSG send;
	memset(&send, 0, sizeof(send));

	DBG("%s\r\n", __func__);
	
	send.stHead.ucType = ZJCT_MNG_TYPE_DT_CONTROL;
	send.stHead.ucDstAddr = ZJCT_DEV_ADDR_DTJKH;
	send.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	send.stHead.usLen = 6;
	send.ucData[0] = radio_id[0];
	send.ucData[1] = glocal_mem_update[0].glocaldev_num;
	send.ucData[2] = radio_ctl_type;
	send.ucData[3] = DTZX_STOP;
	
	if(SEAT_A_TO_B == radio_ZX_type)
	{
		send.ucData[4] = radio_id[1];
		send.ucData[5] = radio_id[0];
	}
	else
	{
		send.ucData[4] = radio_id[0];
		send.ucData[5] = radio_id[1];
	}
/*	
	Zjct_AdptMng_Socket_Send((uint8 *)&send,\
		ZJCT_MNG_MSG_HEADER_LEN + send.stHead.usLen);

	send.ucData[0] = radio_id[1];
	Zjct_AdptMng_Socket_Send((uint8 *)&send,\
		ZJCT_MNG_MSG_HEADER_LEN + send.stHead.usLen);
*/
	/* ��״̬*/
	radio_ZX_type = 0;
	radio_ctl_type = 0;
	radio_ZX_status = DTZX_STOP;

	return DRV_OK;
}

static int32 Zjct_zhuanxin_A_to_B(void)
{
	ST_ZJCT_MNG_MSG send;
	memset(&send, 0, sizeof(send));

	DBG("%s: radio A = %d, radio B = %d\r\n", \
		__func__, radio_id[0], radio_id[1]);
	
	send.stHead.ucType = ZJCT_MNG_TYPE_DT_CONTROL;
	send.stHead.ucDstAddr = ZJCT_DEV_ADDR_DTJKH;
	send.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	send.stHead.usLen = 6;
	send.ucData[0] = radio_id[0];
	send.ucData[1] = glocal_mem_update[0].glocaldev_num;
	send.ucData[2] = DTZX_HANDLE;
	send.ucData[3] = DTZX_START;
	send.ucData[4] = radio_id[1];
	send.ucData[5] = radio_id[0]; 		
/*
	Zjct_AdptMng_Socket_Send((uint8 *)&send,\
		ZJCT_MNG_MSG_HEADER_LEN + send.stHead.usLen);

	send.ucData[0] = radio_id[1];
	Zjct_AdptMng_Socket_Send((uint8 *)&send,\
		ZJCT_MNG_MSG_HEADER_LEN + send.stHead.usLen);
*/
	/*����״̬*/
	radio_ctl_type = DTZX_HANDLE;
	radio_ZX_type = SEAT_A_TO_B;
	radio_ZX_status = DTZX_START;

	return DRV_OK;
}

static int32 Zjct_zhuanxin_B_to_A(void)
{
	ST_ZJCT_MNG_MSG send;
	memset(&send, 0, sizeof(send));

	DBG("%s: radio A = %d, radio B = %d\r\n", \
		__func__, radio_id[0], radio_id[1]);

	send.stHead.ucType = ZJCT_MNG_TYPE_DT_CONTROL;
	send.stHead.ucDstAddr = ZJCT_DEV_ADDR_DTJKH;
	send.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	send.stHead.usLen = 6;
	send.ucData[0] = radio_id[0];
	send.ucData[1] = glocal_mem_update[0].glocaldev_num;
	send.ucData[2] = DTZX_HANDLE;
	send.ucData[3] = DTZX_START;
	send.ucData[4] = radio_id[0];
	send.ucData[5] = radio_id[1]; 		
/*
	Zjct_AdptMng_Socket_Send((uint8 *)&send,\
		ZJCT_MNG_MSG_HEADER_LEN + send.stHead.usLen);

	send.ucData[0] = radio_id[1];
	Zjct_AdptMng_Socket_Send((uint8 *)&send,\
		ZJCT_MNG_MSG_HEADER_LEN + send.stHead.usLen);
*/
	/* ����״̬*/
	radio_ctl_type = DTZX_HANDLE;
	radio_ZX_type = SEAT_B_TO_A;
	radio_ZX_status = DTZX_START;

	return DRV_OK;
}

static int32 Zjct_zhuanxin_Auto_AB(void)
{
	ST_ZJCT_MNG_MSG send;
	memset(&send, 0, sizeof(send));

	DBG("%s: radio A = %d, radio B = %d\r\n", \
		__func__, radio_id[0], radio_id[1]);
	
	send.stHead.ucType = ZJCT_MNG_TYPE_DT_CONTROL;
	send.stHead.ucDstAddr = ZJCT_DEV_ADDR_DTJKH;
	send.stHead.ucSrcAddr = ZJCT_DEV_ADDR_CY1H;
	send.stHead.usLen = 6;
	send.ucData[0] = radio_id[0];
	send.ucData[1] = glocal_mem_update[0].glocaldev_num;
	send.ucData[2] = DTZX_AUTO;
	send.ucData[3] = DTZX_START;
	send.ucData[4] = radio_id[0];
	send.ucData[5] = radio_id[1]; 		
/*
	Zjct_AdptMng_Socket_Send((uint8 *)&send,\
			ZJCT_MNG_MSG_HEADER_LEN + send.stHead.usLen);
	
	send.ucData[0] = radio_id[1];
	Zjct_AdptMng_Socket_Send((uint8 *)&send,\
		ZJCT_MNG_MSG_HEADER_LEN + send.stHead.usLen);
*/
	/* ����״̬*/
	radio_ctl_type = DTZX_AUTO;
	radio_ZX_type = SEAT_AB_AUTO;
	radio_ZX_status = DTZX_START;
	
	return DRV_OK;	
}

static int32 Zjct_SeatRadioMsg_Process(uint8 *buf, uint32 len)
{
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;

    if((NULL == buf)||(0 == len))
    {
        return DRV_ERR;
    }
 
	switch(msg->ucData[DANHU_MSG_BYTE])
	{
		case SEAT_RADIO_ADDR:
			if(((msg->ucData[DANHU_MSG_BYTE + 1] > 0) \
				&&(msg->ucData[DANHU_MSG_BYTE + 1] <= RADIO_NUM_MAX))
				&&((msg->ucData[DANHU_MSG_BYTE + 2] > 0) \
				&&(msg->ucData[DANHU_MSG_BYTE + 2] <= RADIO_NUM_MAX)))
			{
				
				if(msg->ucData[DANHU_MSG_BYTE + 1] == \
					msg->ucData[DANHU_MSG_BYTE + 2])
				{
					ERR("%s: radio zhuanxin addr equel\r\n", __func__);
					break;
				}
				
				radio_id[0] = msg->ucData[DANHU_MSG_BYTE + 1];/* radio A address */
				radio_id[1] = msg->ucData[DANHU_MSG_BYTE + 2];/* radio B address */

				DBG("%s: radio A = %d, radio B = %d\r\n",\
					__func__, radio_id[0], radio_id[1]);
			}
			break;
		case SEAT_STOP:
			if(DTZX_STOP != radio_ZX_status)
			{
				Zjct_zhuanxin_stop();
			}
			break;
		case SEAT_A_TO_B:
			if(DTZX_STOP != radio_ZX_status)
			{
				Zjct_zhuanxin_stop();
			}
			Zjct_zhuanxin_A_to_B();
			break;
		case SEAT_B_TO_A:
			if(DTZX_STOP != radio_ZX_status)
			{
				Zjct_zhuanxin_stop();
			}
			Zjct_zhuanxin_B_to_A();
			break;
		case SEAT_AB_AUTO:
			if(DTZX_STOP != radio_ZX_status)
			{
				Zjct_zhuanxin_stop();
			}
			Zjct_zhuanxin_Auto_AB();
			break;
		default :
			ERR("%s: param error msg->ucData[DANHU_MSG_BYTE] = %d",\
					__func__, msg->ucData[DANHU_MSG_BYTE]);
			break;
	}
	
	return DRV_OK;
}

static int32 zjct_SeatCall_Msg_SendToInc(uint8 *buf, uint32 len)
{
	if((NULL == buf)||(0 == len))
	{
        return DRV_ERR;
	}
	
	switch(buf[5])
	{
		case ZJCT_CALL_REQ:
			Zjct_Local_Mem_Data_Push((uint8 *)&glocal_mem_update[0].data, \
								ZJCT_STATUS_MSG_DATA_LEN);
			
			glocal_mem_update[0].inc_connect = ZJCT_INC_DISCONNECT;
			glocal_mem_update[0].inc_callIn = 2;
			glocal_mem_update[0].data.dev_mode = ZJCT_MODE_BHFS;
			glocal_mem_update[0].data.dev_dscId = 0xf001;
				
			DBG("seat send a callout \r\n");
			break;
		case ZJCT_CALL_ACK:
			DBG("seat send a callin ack \r\n");
			Zjct_Local_Mem_Data_Push((uint8 *)&glocal_mem_update[0].data, \
								ZJCT_STATUS_MSG_DATA_LEN);			
			glocal_mem_update[0].data.dev_mode = ZJCT_MODE_BHJS;
			glocal_mem_update[0].data.dev_dscId = 0xf001;			
			break;
		case ZJCT_CONNECT_REQ:
			glocal_mem_update[0].inc_connect = ZJCT_INC_CONNECT;
			
			if(FMQ_CTRL_OFF != buzzer_status)
			{
				Buzzer_Control(FMQ_CTRL_OFF);
				buzzer_status = FMQ_CTRL_OFF;
			}
			DBG("seat send a callin connect req\r\n");
			break;	
		case ZJCT_RELEASE_REQ:
			glocal_mem_update[0].inc_connect = ZJCT_INC_DISCONNECT;
			glocal_mem_update[0].inc_callIn = 0;
			Zjct_Local_Mem_Data_Pop((uint8 *)&glocal_mem_update[0].data, \
							ZJCT_STATUS_MSG_DATA_LEN);
			
			if(FMQ_CTRL_OFF != buzzer_status)
			{
				Buzzer_Control(FMQ_CTRL_OFF);
				buzzer_status = FMQ_CTRL_OFF;
			}

			DBG("seat send release req\r\n");
			break;						
		case ZJCT_RELEASE_ACK:
			DBG("seat send release ack\r\n");
			break;
		default: 
			break;
	}
	
	//Zjct_IncCall_Socket_Send(buf, len);

	return DRV_OK;

}
/*******************************************************************************
* Function: Zjct_SeatMng_Msg_Process
*
* Description: 
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
static int32 Zjct_SeatMng_Msg_Process(uint8 *buf, uint32 len)
{
	ST_ZJCT_MNG_MSG *msg = (ST_ZJCT_MNG_MSG *)buf;

	switch(msg->stHead.ucType)
	{
		case ZJCT_SEAT_TXB_CMD:
			Zjct_SeatStatusMsg_Process(buf, len);
			break;
		case ZJCT_MNG_TYPE_DT_CONTROL:
			Zjct_SeatRadioMsg_Process(buf , len);
			break;
		case 0x01:/*֮���Ӧ���������*/
			zjct_SeatCall_Msg_SendToInc(buf, len);
			break;
		default: 
			ERR("Zjct_SeatMng_RxThread parma error buf[0] = 0x%x\r\n", \
				buf[0]);
		return DRV_ERR;
			break;
	}
	
	return DRV_OK;
}

/*******************************************************************************
* Function: Zjct_SeatMng_RxThread
*
* Description: ָ��ͨ�Ű���ϯλ�����֮���������ͨ��ʱ���ݴ���
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
void Zjct_SeatMng_RxThread(void)
{
	struct sockaddr_in remote_addr;
	uint8 buf[ZJCT_MAX_DATA_LEN_600];
	int Rec_Len;
	uint32 len;
	
	LOG("Zjct_SeatMng_RxThread start \r\n");
	
	len = sizeof(remote_addr);
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		
		if((Rec_Len = recvfrom(g_ZjctSeatMngSockFd, buf, ZJCT_MAX_DATA_LEN_600, 0,
		   (struct sockaddr *)&remote_addr, &len)) != DRV_ERR)
		{

			//if (0 == g_dbg_port)
			{
				if (g_ZjctSeatMngSockAddr.sin_addr.s_addr != remote_addr.sin_addr.s_addr)
				{
					g_ZjctSeatMngSockAddr.sin_addr.s_addr = remote_addr.sin_addr.s_addr;
				}

				if (g_ZjctSeatMngSockAddr.sin_port != remote_addr.sin_port)
				{
					//g_ZjctSeatMngSockAddr.sin_port = remote_addr.sin_port;
				}
			}
			
			Zjct_SeatMng_Msg_Process(buf, Rec_Len);

		}
	}
}

/*******************************************************************************
* Function: Zjct_SeatMng_Socket_Close
*
* Description: ָ��ͨ�Ű���ϯλ�����֮���������ͨ��Socket�ر�.
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: ��
*
* Comments:     
*******************************************************************************/
static int32 Zjct_SeatMng_Socket_Close(void)
{		
	if (g_ZjctSeatMngSockFd)
	{
		close(g_ZjctSeatMngSockFd);
	}
	
	return DRV_OK;
}

int32 Zjct_Socket_Close(void)
{
	uint8 id = 0; 
	Zjct_AdptVoc_Socket_Close();
	Zjct_AdptMng_Socket_Close();
	
	for(id = 0; id < CHETONG_SEAT_MAX; id++)	
	{
		Zjct_IncVoc_Socket_Close(id);
		Zjct_IncCall_Socket_Close(id);
	}
	
//	Zjct_SeatCall_Socket_Close();
//	Zjct_SeatData_Socket_Close();
	Zjct_SeatMng_Socket_Close();

	return DRV_OK;
}

/*******************************************************************************
* Function: zjct_Socket_init
*
* Description: װ�׳�ͨ����ͻ���ͨ��socket��ʼ��
*
* Inputs: ��
*      
* Outputs: ��
*       
* Returns: DRV_ERR��ʧ�ܣ�DRV_OK ���ɹ�
*
* Comments:     
*******************************************************************************/
/*100ms*/
int Zjct_Mng_Thread(const void *data)
{
	static 	uint32 Cnt2s = 0;
	static 	uint32 Cnt3s = 0;
	static 	uint32 Cnt3s_flg[CHETONG_SEAT_MAX] = {0};
	static 	uint32 Cnt4s = 0;
	static 	uint32 Cnt5s = 0;

	uint8 id = 0;


	//LOG("%s start \r\n", __func__);
	
	if(FLG_OK == GET_SOCKTINIT_FLG())
	{	
		/* 2 s����һ������*/
		Cnt2s++;
		if(20 == Cnt2s)
		{
			for(id = 0; id < CHETONG_SEAT_MAX; id++)
			{
				Zjct_AdptMng_AutoSendStatusMsg(id);
			}
			
			if(1 == gauto_send_cfg_flg)
			{
				Zjct_AdptMng_AutoSend_Open_GroupCall();
			}
			
			if(1 == gdanhu_timeout_flg)
			{
				Zjct_DanHu_Status_TimeOut_Check();
			}
			
			Cnt2s = 0;
		}
		
	
		Cnt3s++;
		if (30 == Cnt3s)
		{
			/* ����ͨ������3s ��ʱ��Ϣ*/
			for(id = 0; id < CHETONG_SEAT_MAX; id++)
			{		
				if (FLG_OK != Cnt3s_flg[id])
				{
					if  (FLG_OK != GET_NETREG_FLG(id) ) //3
					{
						/* ������������*/
						Zjct_AdptMng_SendRegisterNetwork_Req(id);
					}			
					else if ((FLG_OK == GET_NETREG_FLG(id))\
						&&(FLG_OK != GET_PARAMREQ_FLG(id)))
					{
						/* ���Ͳ�������*/
						Zjct_AdptMng_SendParamConfig_Req(id);			
					}
					else if ( (FLG_OK == GET_NETREG_FLG(id))\
						&&(FLG_OK == GET_PARAMREQ_FLG(id)) )
					{
						 Cnt3s_flg[id] = FLG_OK;
					}
				}
			}
			
			Cnt3s = 0;
		}

		Cnt4s++;
		if(40 == Cnt4s)
		{	
			Zjct_Handle_Status_Check();
			Cnt4s = 0;
		}
		
		Cnt5s++;
		if(50 == Cnt5s)
		{	
			Zjct_AdptMng_Update_MemList_OffLine_Msg();
			Zjct_AdptMng_Update_Radio_OffLine_Msg();
			Cnt5s = 0;
		}
	}
		
	return 1;
}



static int32 Zjct_Socket_init(void)
{	
	uint8 id = 0; 
	g_ZjctAdpterAddr.sin_family = AF_INET;
	g_ZjctAdpterAddr.sin_port = htons(II_Adpt_remote.zjct_adptmng_port);
//	g_ZjctAdpterAddr.sin_addr.s_addr = htonl(0xc0a80b01); //192.168.111.1
//	g_ZjctAdpterAddr.sin_addr.s_addr = htonl((glocaladpt_ipaddr & 0xffffff00) | 0x01); //192.168.11.1
	g_ZjctAdpterAddr.sin_addr.s_addr = htonl(II_Adpt_remote.ipaddr);

/* ���͹㲥����*/
	g_ZjctAdptMngSockAddr.sin_family = AF_INET;
	g_ZjctAdptMngSockAddr.sin_port = htons(II_Adpt_remote.zjct_adptmng_port);
	g_ZjctAdptMngSockAddr.sin_addr.s_addr = htonl((II_Adpt_local[0].ipaddr & 0xffffff00) | 0xff); //192.168.11.255
	
/*  ���͹㲥����*/
	g_ZjctAdptVocSockAddr.sin_family = AF_INET;
	g_ZjctAdptVocSockAddr.sin_port = htons(II_Adpt_remote.zjct_adptvoc_port);
	g_ZjctAdptVocSockAddr.sin_addr.s_addr = htonl((II_Adpt_local[0].ipaddr &0xffffff00) | 0xff);

	g_ZjctIncVocSockAddr.sin_family = AF_INET;
	g_ZjctIncVocSockAddr.sin_port = htons(II_Inc_remote.zjct_incvoc_port);
//	g_ZjctIncVocSockAddr.sin_addr.s_addr = inet_addr("192.168.8.18"); //��ǰ����ͨ��������IP
	g_ZjctIncVocSockAddr.sin_addr.s_addr = htonl(II_Inc_remote.ipaddr);

	g_ZjctIncCallSockAddr.sin_family = AF_INET;
	g_ZjctIncCallSockAddr.sin_port = htons(II_Inc_remote.zjct_incsem_port);
//	g_ZjctIncCallSockAddr.sin_addr.s_addr = inet_addr("192.168.8.18"); 
	g_ZjctIncCallSockAddr.sin_addr.s_addr = htonl(II_Inc_remote.ipaddr);
	
/* ��¼Զ��ϯλ�������socket��Ϣ�����ڽ����߳��н����ж�*/
	g_ZjctSeatMngSockAddr.sin_family = AF_INET;
	g_ZjctSeatMngSockAddr.sin_port = htons(II_Seat_remote.seat_mng_port); /* 45000 */
	g_ZjctSeatMngSockAddr.sin_addr.s_addr = htonl(II_Seat_remote.ipaddr);

	/* ָ��ͨ�Ű��복ͨ����������ͨ��socket*/
	if(DRV_OK != Zjct_AdptMng_Socket_init())
	{
		ERR("Zjct_AdptMng_Socket_init failed!\r\n");
		return DRV_ERR;
	}

	/* ָ��ͨ�Ű��복ͨ����������ͨ��socket*/
	if (DRV_OK != Zjct_AdptVoc_Socket_init())
	{	
		ERR("Zjct_AdptVoc_Socket_init failed!\r\n");
		return DRV_ERR;
	}	

	for(id = 0; id < CHETONG_SEAT_MAX; id++)
	{
		if(DRV_OK != Zjct_AdptMng_Socket_init_ID(id))
		{
			ERR("Zjct_AdptMng_Socket_init_ID failed!\r\n");
			return DRV_ERR;
		}

		if(DRV_OK != Zjct_AdptVoc_Socket_init_ID(id))
		{
			ERR("Zjct_AdptVoc_Socket_init_ID failed!\r\n");
			return DRV_ERR;
		}		
		
		/* ָ��ͨ�Ű����������������ͨ��socket */
		if(DRV_OK != Zjct_IncCall_Socket_init_ID(id))
		{
			ERR("Zjct_IncCall_Socket_init_ID failed!\r\n");
			return DRV_ERR;
		}

		/* ָ��ͨ�Ű������������ͨ��socket */
		if(DRV_OK != Zjct_IncVoc_Socket_init_ID(id))
		{
			ERR("Zjct_IncVoc_Socket_init_ID failed!\r\n");
			return DRV_ERR;
		}
	}
	
#if 0
	/* ָ��ͨ�Ű��ϯλ֮��ͨ��socket */
	if (DRV_OK != Zjct_SeatMng_Socket_init())
	{
		ERR("Zjct_SeatMng_Socket_init failed!\r\n");
		return DRV_ERR;
	}
	
	if (DRV_OK != Zjct_SeatData_Socket_init())
	{
		ERR("Zjct_SeatData_Socket_init failed \r\n");
		return DRV_ERR;
	}
			
	/* ָ��ͨ�Ű��ϯλ�˻�������ͨ��socket */
	if (DRV_OK != Zjct_SeatCall_Socket_init())
	{	
		ERR("Zjct_SeatCall_Socket_init failed!\r\n");
		return DRV_ERR;
	}
#endif	

	return DRV_OK;
}

int32 Zjct_init(void)
{	
	uint8 id = 0;

	/* ��ʼ��Mutex Lock */

	/*��ʼ��Զ�˳�Աһ�ź�����*/
	memset(gremote_mem_update, 0, sizeof(gremote_mem_update));
	memset(gremote_mem_save, 0, sizeof(gremote_mem_save));

	/*��ʼ����̨����*/
	memset(gremote_radio_save, 0, sizeof(gremote_radio_save));
	memset(gremote_radio_update, 0, sizeof(gremote_radio_update));

	/**/
	memset(&glocal_incmem, 0, sizeof(glocal_incmem));
	memset(&adapter_voc_buf, 0, sizeof(adapter_voc_buf));
	
	if(CHAN_CFG == g_chan_auto_cfg)	
	{
		memset(g_chan, CHAN_FREE, sizeof(g_chan));
	}
	
	/*��ʼ�����س�Աһ�ź�����*/
	memset(&glocal_mem_update, 0, sizeof(glocal_mem_update));
#ifdef SINGLE_MODE_710
	memset(&glocal_mem_save, 0, sizeof(glocal_mem_save));
#else
	memset(glocal_mem_stack, 0, sizeof(glocal_mem_stack));
#endif

	for(id = 0; id < CHETONG_SEAT_MAX; id++)
	{
		glocal_mem_update[id].glocaldev_num = get_config_cat_var_val(config_cfg, ETH, "localdev_num") + id;

		glocal_dev_default_data[1] = glocal_mem_update[id].glocaldev_num;
		*((uint16 *)&glocal_dev_default_data[6]) = ghw_ver;
		*((uint32 *)&glocal_dev_default_data[8]) = gsw_ver;	
		memcpy(&glocal_mem_update[id].data, glocal_dev_default_data, \
					ZJCT_STATUS_MSG_DATA_LEN);	
		Zjct_Set_Bit((uint8 *)&online_mem, glocal_mem_update[id].glocaldev_num - 1);

		glocal_mem_update[id].Ipaddr = II_Adpt_local[id].ipaddr;

		glocal_mem_update[id].Netmask= 24;		
	}

	/* ��ʼ��װ�׳�ͨ�й�socket */
	if(DRV_OK != Zjct_Socket_init())
	{
		ERR("Zjct_Socket_init failed \r\n");
		return DRV_ERR;
	}
	else
	{
		/*����������������������Ϣ*/
		//Zjct_AdptMng_SendRegisterNetwork_Req();

		/*��������INC  ���Ϳ�ͨ�û���Ϣ*/
		//Zjct_IncCall_SendOpenUserMsg();		
	}

	LOG("Zjct_Socket_init OK\r\n");
	
	usleep(10000);
	
	SET_SOCKTINIT_FLG();
	
	LOG(" Zjct_init OK !\r\n");
	
	return DRV_OK;
}

#if defined(__cplusplus) 
} 
#endif 

#endif // __ZJCT_PROC_C__

