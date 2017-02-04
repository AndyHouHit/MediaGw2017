/****************************************************************************************************
* Copyright @,  2012-2017,  LCTX Co., Ltd. 
* Filename:     zjct_proc.c  
* Version:      1.00
* Date:         2012-04-20	    
* Description:  Z024��Яʽָ��ͨ���ն�II��ָ��ͨ�Ű���װ�׳�ͨ����ͨ��Э�鴦��
			<20120315_ZJCT-002װ�׳�ͨ����ͨ�Ź淶.doc>
* Modification History: ��ʯ�� 2012-04-20 �½��ļ�

*****************************************************************************************************/
//header: zjct_proc.h
#ifndef __ZJCT_PROC_H__
#define __ZJCT_PROC_H__

#if defined(__cplusplus)
extern "C" 
{ 
#endif

/*****************************************************************************************************
 * �궨��
*****************************************************************************************************/

#define IN 
#define OUT

/* װ�׳�ͨϵͳ���豸������II��ϯλ���32����ͨ���豸���13 
(����12����̨��1�����Ŀ��ƺ�)��Ԥ��11����չ��

����Ϊ:���Ŀ��ƺУ���̨0~12, II 0~31
*/

#undef	ZJCT_DEV_TYPE_MAX
#define ZJCT_DEV_TYPE_MAX 			12 	//12�����͵ĺ�?

#undef 	ZJCT_DEV_INDX_MAX
#define ZJCT_DEV_INDX_MAX 			32	//ÿ�����ͺ������32��

#undef 	LOCAL_INCMEM_NUM
#define LOCAL_INCMEM_NUM		1   /*����INC ��Ա����*/

#undef 	REMOTE_INCMEM_NUM
#define REMOTE_INCMEM_NUM		10 /*Զ��INC ��Ա����*/


/* ���ܺͻ���ͨ�ŵ�udp���ݰ�����ͷ*/
//�淶�й涨1467�ֽڣ���ʵ��ʱΪ��ʡ�ռ䣬����600�ֽ�
#undef 	ZJCT_MAX_DATA_LEN_600
#define ZJCT_MAX_DATA_LEN_600			600 

//�淶��δ��ȷ�涨����ʱȡ�����Ϣ���Ȳ���128 �ֽ�
#undef 	ZJCT_MAX_CALLSEM_DATA_LEN
#define ZJCT_MAX_CALLSEM_DATA_LEN		ZJCT_MAX_DATA_LEN_600

#define ZJCT_VOC_G711_20MS_LEN_160  	160

#define ZJCT_INC_VOC_20MS_LEN_166 		(160 + 6)

#define ZJCT_VOC_PCM_20MS_LEN_320  		(2 * ZJCT_VOC_G711_20MS_LEN_160)

//ÿ20ms����һ��328�ֽڵĻ���������������320��
#define ZJCT_PKGVOC_LEN_328  			(ZJCT_VOC_PCM_20MS_LEN_320 + 8)  

#define RTP_HEADER_SIZE  				12
#define RTP_RXBUFF_SIZE  				(ZJCT_VOC_G711_20MS_LEN_160 + RTP_HEADER_SIZE)
#define RTP_TXBUFF_SIZE  				(ZJCT_VOC_G711_20MS_LEN_160 + RTP_HEADER_SIZE)
#define RTP_G711_MSG_LEN_172			(ZJCT_VOC_G711_20MS_LEN_160 + RTP_HEADER_SIZE)


/*��Աһ�ź�״̬֡����*/
#define ZJCT_STATUS_MSG_LEN 			(ZJCT_MNG_MSG_HEADER_LEN + \
										ZJCT_STATUS_MSG_DATA_LEN) /* 34 */
										
	
#define ZJCT_STATUS_MSG_DATA_LEN  		0x1d  		/* 29 */
#define ZJCT_STATUS_MSG_UPDATE_DATA_LEN 0x0d 	/* 13 */

#define RADIO_NUM_PER_DEV 				0x04
#define RADIO_DEV_NUM					0x02
#define RADIO_NUM_MAX					(RADIO_NUM_PER_DEV * RADIO_DEV_NUM)

#define VOICE_TIME_1min		(50 * 2)
#define VOICE_MAX_LEN		(ZJCT_VOC_PCM_20MS_LEN_320 * (VOICE_TIME_1min))
#define VOICE_MEM_MAX		(ZJCT_DEV_INDX_MAX)
#define VOICE_RADIO_MAX		(RADIO_NUM_MAX)

/*
װ�׳�ͨ�豸�ڲ�������������UDP/IP֮�ϵ�Ӧ�ò�Э�飬
����IP/UDP��װ������ͻ������ò�ͬ��UDP�˿ںţ�
����UDP�˿ں�0x6600��26112��������UDP�˿ں�0x6601��26113����
*/
//����ͨ���ö˿ڣ�����ͨ����Ҫ����ϵͳ����
#define ZJCT_ADAPTER_MNG_PORT			(0x6600)
//�������� �˿ں�
#define ZJCT_ADAPTER_VOICE_PORT 		(0x6601)

/*  ��INCͨ���ö˿ںŻ�������˿ں�*/
#define ZJCT_INC_SEM_PORT				(30000)
/*  ��INCͨ���ö˿ںŻ������ݶ˿ں�*/
#define ZJCT_INC_VOC_PORT				(30001)

#define ZJCT_CALL_TO_PC 				(45000)
#define ZJCT_TESTVOC_LEN 				960000



#define ZJCT_CALL_CFG_PER_USERINFO_LEN 	16
#define	SIP_MAXNUMLEN				8  /*���������鳤��*/

#define ZJCT_PRO_TYPE 				0x01 //���͵�ǰ����Ϊ1
#define ZJCT_PRO_VER				0x01  //�ְ汾Ϊ1

/* ��ͨ�뻰������Э���ʽ��1������������Ϣ��2������������Ϣ*/
#define ZJCT_INC_MNG 				0x00 
#define ZJCT_INC_CONFIG 			0x01 
#define ZJCT_INC_VOICE_CMD  		0x02
#define ZJCT_INC_VOICE_DATA      	0x03

#undef	ZJCT_XW_MIN_INDX
#define	ZJCT_XW_MIN_INDX 			1

#undef	ZJCT_XW_MAX_INDX
#define	ZJCT_XW_MAX_INDX 			16


#define PERIOD_4s  					40
#define PERIOD_3S					30
#define PERIOD_15S					150

#define ZXKZH_TYPE_IDX 				0x00
#define CY1HH_TYPE_IDX				0x03
#define CY2HH_TYPE_IDX 				0x04

#define CY1HH_DEV_IDX_MEM8			(0x07)/*(8 -1)*/

#define ZJCT_BROADCASE_ADDR 		0xfeff

#define ZJCT_LOCALDEVID_OFFSET 		0x01
#define ZJCT_MODE_OFFSET 	  		0x03
#define ZJCT_DSTDEVID_OFFSET 	  	0x04

#define ZJCT_LOCALMEM_STACK_CNT		0x05		/*��󱣴汾��״̬����*/


#define FLG_OK 			1
#define FLG_FAILED 		0

#define GET_NETREG_FLG(id)  		(gsend_register[id])
#define SET_NETREG_FLG(id)  		(gsend_register[id] = FLG_OK)
#define CLR_NETREG_FLG(id)  		(gsend_register[id] = FLG_FAILED)

#define GET_PARAMREQ_FLG(id)  	(grequest_status[id])
#define SET_PARAMREQ_FLG(id)  	(grequest_status[id] = FLG_OK)
#define CLR_PARAMREQ_FLG(id) 		(grequest_status[id] = FLG_FAILED)

#define GET_SOCKTINIT_FLG()  		gsocketinit_status
#define SET_SOCKTINIT_FLG()  		gsocketinit_status = FLG_OK
#define CLR_SOCKTINIT_FLG() 		gsocketinit_status = FLG_FAILED

#define GET_OPENUSERMSG_FLG()  	gopenusermsg_status
#define SET_OPENUSERMSG_FLG()  	gopenusermsg_status = FLG_OK
#define CLR_OPENUSERMSG_FLG()  	gopenusermsg_status = FLG_FAILED

#define GET_USERNUMREG_FLG()		gusernum_status
#define SET_USERNUMREG_FLG()		gusernum_status = FLG_OK
#define CLR_USERNUMREG_FLG()		gusernum_status = FLG_FAILED

#define ADAPTER_INC_ADDR 	0xc0a80800  /*192.168.8.0*/
#define ADAPTER_ADDR 		0xc0a80b00	/*192.168.11.0*/


#define	PTT_DOWN	0x00//PTT����ֵ
#define	PTT_UP		0x01//PTT����ֵ



/*****************************************************************************************************
 * �ṹ�嶨��
*****************************************************************************************************/
/* RTP header */
struct TRTPHeader
{	
	unsigned       RTPVer    :2; /*RTP version (2)*/	
	unsigned       RTPPadding:1; /*RTP padding (0)*/
	unsigned       RTPExt    :1; /*RTP extension (0)*/
	unsigned       RTPCSRCCnt:4; /*RTP CSRC count (0)*/  
	unsigned       RTPMarker :1; /*RTP marker (0)*/
	unsigned       RTPPT     :7; /*RTP payload type*/	
	unsigned       RTPSeqNum :16;    /*RTP sequence number*/
	unsigned       RTPTimeStamp:32; /*RPT Time stamp*/
	unsigned       RTPSSRC  :32;   /*RTP synchronization source identifier*/
};

enum __PANEL_STATUS__
{
	PANEL_OFFLINE = 0x00,
	PANEL_ONLINE 
};

typedef enum fmq_control {
    FMQ_CTRL_OFF = 0x0,
    FMQ_CTRL_SLOW = 0x1,
    FMQ_CTRL_FAST = 0x2,
	FMQ_CTRL_BUTT
}EN_FMQ_CTRL;

/* װ�׳�ͨ�ڲ���������֡�ṹ�������Ͷ���*/
typedef enum en_zjct_mng_type
{
	ZJCT_MNG_TYPE_PARAMREQ = 0x60,
	ZJCT_MNG_TYPE_PARAMCFG = 0x61,
	ZJCT_MNG_TYPE_PARAMCFGACK = 0x62,

	ZJCT_MNG_TYPE_REPORTSTATUS = 0x63,
	ZJCT_MNG_TYPE_REPORTSTATUSACK = 0x63,
	
	ZJCT_MNG_TYPE_PARAMUPADATE = 0x64,
	ZJCT_MNG_TYPE_PARAMUPADATEACK = 0x65,

	ZJCT_MNG_TYPE_QUERYSTATUS = 0x66, //??

	ZJCT_MNG_TYPE_REGREQ = 0x6f,
	ZJCT_MNG_TYPE_REGREQACK = 0x6f,
	
	ZJCT_MNG_TYPE_PCM64K = 0x89,//64k PCM����
	ZJCT_MNG_TYPE_PCM128K = 0x8b,//128k PCM����
 
	ZJCT_MNG_TYPE_CONFIG = 0x90,//��������
	ZJCT_MNG_TYPE_CONFIG_ACK = 0x91,//��������Ӧ��

	ZJCT_MNG_TYPE_DEV_STATUS_INFO = 0x92,//�豸״̬��Ϣ

	ZJCT_MNG_TYPE_DT_CONTROL = 0x93,//��̨����
	ZJCT_MNG_TYPE_DT_CONTROL_ACK = 0x94,//��̨����Ӧ��
/*
	ZJCT_SEAT_TXB_CMD = 0x98
*/
	SEM_TYPE_BUTT 
}EN_ZJCT_MNG_TYPE;

#define ZJCT_SEAT_TXB_CMD  ZJCT_MNG_TYPE_DEV_STATUS_INFO

/* װ�׳�ͨ�豸��ַ����*/
typedef enum en_zjct_dev_addr
{
	ZJCT_DEV_ADDR_ZHZXH = 0xF0,	//ָ�������Ŀ��ƺ�
	ZJCT_DEV_ADDR_ZDZXH = 0xF0,	//ս�������Ŀ��ƺ�
 
	ZJCT_DEV_ADDR_DTJKH = 0xF1,	//��̨�ӿ��豸
	ZJCT_DEV_ADDR_CY1H = 0xF4,	//��Ա1�ź�(��Яʽָ��ͨ���ն�II��ʹ�õĵ�ַ)
	ZJCT_DEV_ADDR_CY2H = 0xF5,	//��Ա2�ź�

	ZJCT_DEV_ADDR_YHJKH = 0xF6,	//�û��ӿں�
	ZJCT_DEV_ADDR_YHKZH = 0xF7,	//�û���չ��
	ZJCT_DEV_ADDR_ZY1H = 0xF8,	//��Ա1�ź�

	ZJCT_USER_ADDR = 0xFE,
	ZJCT_DEV_ADDR_BRDCAST = 0xFF,

	ZJCT_DEV_ADDR_BUTT
}EN_ZJCT_DEV_ADDR;

/*
��	������ʽ	����	����Ŀ��	����
1	ǿ������	0x01	0xFE FF����Ա�㲥��	��Ա�ն�
2	ǿ������	0x02	ͬ��	��Ա�ն�
3	���ڹ㲥	0x03	ͬ��	��Ա�ն�
4	��ͨ���	0x04	0xF4 01��0A����Ա1��10����ͨ��������ߵ�ַ��ţ�	��Ա1�ź�
5	����			0x05	0xF4 01��0A����Ա1��10��	��Ա1�ź�
6	���Ƶ�̨����	0x06	0xF1 01��0B����̨1~12��	��Ա�ն�
7	���Ƶ�̨����	0x07	ͬ��	��Ա�ն�
8	��������	0x08	0xF1 01��0B����̨1~12��	��Ա�ն�
9	��������	0x09	ͬ��	��Ա�ն�
10	����/��������	0x0A	0xF0�����Ŀ��ƺ�I�ͣ�	��Ա1�ź�
11	����/��������	0x0B	ͬ��	��Ա1�ź�
*/

typedef enum _zjct_work_mode_type
{
	ZJCT_MODE_NULL = 0x00,
	ZJCT_MODE_QHFS = 0x01,
	ZJCT_MODE_QHJS = 0x02,
	ZJCT_MODE_CNGB = 0x03,
	ZJCT_MODE_CTZH = 0x04,
	ZJCT_MODE_DH = 0x05,
	ZJCT_MODE_KZDTFS = 0x06,
	ZJCT_MODE_KZDTJS = 0x07,
	ZJCT_MODE_GTFS = 0x08,
	ZJCT_MODE_GTJS = 0x09,
	ZJCT_MODE_BHFS = 0x0A,
	ZJCT_MODE_BHJS = 0x0B,
	ZJCT_MODE_DTFS = 0x0C,
	ZJCT_MODE_DTJS = 0x0D,
	ZJCT_MODE_DTSDZXFS = 0x0E,
	ZJCT_MODE_DTSDZXJS = 0x0F,
	ZJCT_MODE_DTZDZXFS = 0x10,
	ZJCT_MODE_DTZDZXJS = 0x11,
	ZJCT_MODE_DTZDZX   = 0x12,
	ZJCT_MODE_SETTING  = 0x14,
	ZJCT_MODE_INQUIRE_MEM  = 0x15,
	ZJCT_MODE_HANDLE   = 0x16,
	ZJCT_MODE_WORKMODE = 0x17,
	ZJCT_MODE_INQUIRE_RADIO = 0x18,
	ZJCT_MODE_TERMINAL_CLASH = 0x19,

	ZJCT_MODE_BUTT, 
}ZJCT_WORKMODE_EN;

/* 
  װ�׳�ͨ���й��̲����������Ͷ���
*/
typedef enum _zjct_call_config_type
{
	ZJCT_CALL_CFG_USEROPEN_REQ = 0x01, 	//5.1.2	��ͨ��Ϣ

	ZJCT_CALL_CFG_USERINFO_INJECT,		//5.1.3	�û���Ϣע����Ϣ
	ZJCT_CALL_CFG_USERINFO_INJECT_ACK,		

	ZJCT_CALL_CFG_USERPHONENUM_REGISTER_REQ,	//5.1.4	�û�����ע����Ϣ
	ZJCT_CALL_CFG_USERPHONENUM_REGISTER_ACK,	

	ZJCT_CALL_CFG_USERPHONENUM_UNREGISTER_REQ,	//5.1.5	�û�����ע����Ϣ
	ZJCT_CALL_CFG_USERPHONENUM_UNREGISTER_ACK,
	
	ZJCT_CALL_CFG_BUTT
}ZJCT_CALL_CONFIG_TYPE;

/* 
  װ�׳�ͨ������Ϣ���Ͷ���
*/
typedef enum _zjct_call_sem_type
{
	ZJCT_CALL_REQ = 0x01, 	//5.2.1	��������
	ZJCT_CALL_ACK,			//5.2.2	����Ӧ��
	ZJCT_CONNECT_REQ,		//5.2.3	��������		
	ZJCT_RELEASE_REQ,		//5.2.4	�ͷ�����
	ZJCT_RELEASE_ACK,		//5.2.5	�ͷ�֤ʵ
	
	ZJCT_PTT_SEM = 0x0a,//5.2.6	PTT������Ϣ
	
	ZJCT_CALL_SEM_BUTT
}ZJCT_CALL_SEM_TYPE;

typedef enum _zjct_inc_connect_status_type
{
	ZJCT_INC_DISCONNECT = 0x00,
	ZJCT_INC_CONNECT 	= 0x01,
}ZJCT_INC_CONNECT_STATUS_TYPE;

typedef enum _zjct_connect_type
{
	ZJCT_CHOOSE_CALL = 0x01,
	ZJCT_YYQSGHY_CALL = 0x02,
	ZJCT_SJQSGHY_CALL = 0x03,
	ZJCT_GROUNP_CALL = 0x04,
	ZJCT_RADIO_CALL = 0x05
}ZJCT_CONNECT_TYPE;

typedef enum _zjct_release_reason
{
	ZJCT_NORMAL_RELEASE = 0x01,
	ZJCT_DEVICE_ERROR = 0x02,
	ZJCT_TIMEOUT_RELEASE = 0x03
}ZJCT_RELEASE_REASON;

typedef enum _zjct_encode_type
{
	ZJCT_ENCODE_G729 = 0x01,
	ZJCT_ENCODE_CVSD = 0x02,
	ZJCT_ENCODE_G729_CVSD = 0x03,
	ZJCT_ENCODE_PCM = 0X04
}ZJCT_ENCODE_TYPE;


/*
	װ�׳�ͨ�������ö���
*/
typedef enum _zjct_param_config_type
{
	ZJCT_SEND_MSG_CONTROL 		= 0x03,		//�������㲥
	ZJCT_LOWER_MEM_LISTEN_RANGE = 0x04,		//����������
	ZJCT_SUBNET_IP_ADDR			= 0x05,		//����������
	ZJCT_GROUPCALLMEM_START		= 0x06,		//�������㲥
	ZJCT_GROUPCALLMEM_END		= 0x07,		//�������㲥
	ZJCT_LOWER_M_VALUE			= 0x08,		//����������
	ZJCT_LOWER_MEM_SENDTO		= 0x09,		//����������
	ZJCT_HIGH_PERMISSION		= 0x0a,		//����������
	ZJCT_LOWER_PARAM_INQUIRE	= 0x0b		//����������
}ZJCT_PARAM_CONFIG_TYPE;

/*
	"��ͨ���ſ���"�������������ò�������
*/
typedef enum _zjct_send_msg_control
{
	ZJCT_SEND_MSG_CTL_RECEIVE 	= 0x01,
	ZJCT_SEND_MSG_CTL_LEADER	= 0x02,
	ZJCT_SEND_MSG_CTL_ALL		= 0x03
}ZJCT_SEND_MSG_CTL;

/*
	"�ͼ���Ա����������Χ"�������������ò�������
*/
#if 0
typedef struct _zjct_lower_memlisten
{
	uint8 lowermemlisten[12];	
}__attribute__((packed))ZJCT_LOWER_MEMLISEN;
#endif
typedef enum _zjct_lower_mem_listen_range
{
	RADIO_NUM1 = 0x01,
	RADIO_NUM2 = 0x02,
	RADIO_NUM3 = 0x03,
	RADIO_NUM4 = 0x04,
	RADIO_NUM5 = 0x05,
	RADIO_NUM6 = 0x06,
	RADIO_NUM7 = 0x07,
	RADIO_NUM8 = 0x08,
	RADIO_NUM9 = 0x09,
	RADIO_NUM10 = 0x0a,
	RADIO_NUM11 = 0x0b,
	RADIO_NUM12 = 0x0c	
}ZJCT_LOWER_MEMLISEN;


/*
"����IP��ַ"�������������ò�������
*/
typedef struct _zjct_subnet_ipaddr
{
	uint32	ipaddr;
	uint8	netmask_bit;
}__attribute__((packed)) ZJCT_SUBIPADDR;

/*
"�����ͨ"�������������ò�������
*/
typedef enum _zjct_groupcallmem_start
{
	GROUPCALL_MEM1 = 0x01,
	GROUPCALL_MEM2 = 0x02,
	GROUPCALL_MEM3 = 0x03,
	GROUPCALL_MEM4 = 0x04,
	GROUPCALL_MEM5 = 0x05,
	GROUPCALL_MEM6 = 0x06,
	GROUPCALL_MEM7 = 0x07,
	GROUPCALL_MEM8 = 0x08,
	GROUPCALL_MEM9 = 0x09,
	GROUPCALL_MEM10 = 0x0a,
	GROUPCALL_MEM11 = 0x0b,
	GROUPCALL_MEM12 = 0x0c,
	
}ZJCT_GROUPCALLSTART;

/*
"�߼�Ȩ��"�������������ò�����������
*/
typedef enum _zjct_high_permission_type
{
		HIGH_PERMISSION_SINGLE_CALL	= 0x01,
		HIGH_PERMISSION_GROUP_CALL	= 0x02,
		HIGH_PERMISSION_TRANSMITE	= 0x03,
		HIGH_PERMISSION_SETTING		= 0X04
}ZJCT_HIGH_PERMISSION_TYPE;

typedef enum _zjct_permission_status
{
	PERMISSION_FORBID	= 0x00,
	PERMISSION_ALLOW	= 0x01
}ZJCT_PERMISSION_STATUS;

/*
"�ͼ���Ա������ѯ"�������������ò�������
*/
typedef enum _zjct_lower_param_inquire_type
{
	M_VAULE_INQUIRE	= 0x01,
	GROUP_LISTEN_RANGE_IRQUIRE	= 0x02,
	GROUP_LISTEN_SENDTO_IRQUIRE	= 0x03
}ZJCT_PARAMTYPE;

typedef enum _file_status
{
	FILE_CLOSE = 0,
	FILE_OPEN    = 1
}FILE_STATUS;

/*****************************************
 *  װ�׳�ͨ�ڲ��û�ͨ����Ϣ����
*****************************************/



/*****************************************
 *  װ�׳�ͨ��INCͨ����Ϣ����
*****************************************/


/* װ�׳�ͨ�豸�ڲ���������ͷ����װ��ʽ����*/
typedef struct _zjct_mng_header
{
	uint8 ucType;//���������ǳ�ͨ�ڲ�����������ײ���ʶ������1�ֽ�
	uint8 ucDstAddr; //Ŀ���豸��ַ�ǳ�ͨ�ڲ����������Ŀ��ͨ���ն˵��߼��豸��ַ������1�ֽ�
	uint8 ucSrcAddr; //Դ�豸��ַ�ǳ�ͨ�ڲ����������Դͨ���ն˵��߼��豸��ַ������1�ֽڡ�
	uint16 usLen;//������ǳ�ͨ�ڲ�������������ݲ��ֵĳ��ȣ�����2�ֽڣ�ȡֵ��Χ0��1472
}__attribute__((packed)) ST_ZJCT_MNG_HEADER;

#define ZJCT_MNG_MSG_HEADER_LEN sizeof(ST_ZJCT_MNG_HEADER)

typedef struct _zjct_radio_mem_
{
	uint8 	dev_srcId;
	uint8	dev_prio;
	uint8	dev_mode;
	uint16	dev_dscId;
}__attribute__((packed))ZJCT_RADIO_MEM;

typedef struct _zjct_radio_status_msg
{
	ST_ZJCT_MNG_HEADER stHead;	//����ͷ��
	uint8 cfg_status;
	ZJCT_RADIO_MEM radio_mem[RADIO_NUM_PER_DEV];
	uint16 	hw_ver;
	uint32	sw_ver;
}__attribute__((packed))ZJCT_RADIO_STATUS_MSG;

typedef struct _zjct_radio_status_
{
	ZJCT_RADIO_MEM mem_info;
	uint32 	time_cnt;
	uint8 	online;
	uint8	radio_save;
	uint8	radio_get;
	uint16	hw_ver;
	uint32	sw_ver;
	uint32	ip_addr;
	uint8	net_mask;
	int8 		chan_id;
	uint8 	radio_voc_flg;
	uint8 	radio_voc[VOICE_TIME_1min][ZJCT_VOC_PCM_20MS_LEN_320];  /* remote radio voc data*/	
}__attribute__((packed))ZJCT_RADIO_STATUS;

/* װ�׳�ͨ�豸�ڲ�����������Ϣ��������*/
typedef struct _zjct_mng_msg
{
	ST_ZJCT_MNG_HEADER stHead;	//����ͷ��
	uint8 ucData[ZJCT_MAX_DATA_LEN_600];//��������,��ǰ���֧��600�ֽ�
}__attribute__((packed)) ST_ZJCT_MNG_MSG;

#define MAX_MNG_MSG_LEN	sizeof(ST_ZJCT_MNG_MSG)


typedef struct _BOARD_SEM_HEAD
{
	uint8 ProType;		//Э������ 1
	uint8 Edition;		//�汾 1
	uint8 SemType;		//��Ϣ���� 0~2
	uint16 Len;
}__attribute__((packed)) ST_BOARD_SEM_HEAD;
typedef struct _BOARD_SEM
{	
	ST_BOARD_SEM_HEAD Head;
	uint8 Data[ZJCT_MAX_DATA_LEN_600];	
}__attribute__((packed)) ST_BOARD_SEM;


typedef struct _PHONE_SEM_HEAD
{
	uint8 ProType;		//Э������ 1
	uint8 SemType;		//��Ϣ���� 0~2
	uint8 Len;
}__attribute__((packed)) ST_PHONE_SEM_HEAD;
typedef struct _PHONE_SEM
{	
	ST_PHONE_SEM_HEAD Head;
	uint8 Data[ZJCT_MAX_DATA_LEN_600];	
}__attribute__((packed)) ST_PHONE_SEM;

/*****************************************
 * װ�׳�ͨ����ͨ����Ϣ����
*****************************************/
/* ��ͨ�뻰������Э����Ϣͷ*/
typedef struct _zjct_call_sem_header
{
	uint8 ucProType;	//Э������ 1
	uint8 ucProVer;		//��ʾЭ��İ汾�ţ�����Э�齫�����޸ĺ���չ���ְ汾Ϊ1
	uint8 ucSemType;	//1������������Ϣ��2������������Ϣ
	uint16 usLen;		//��ʾ���ĵĳ��ȣ�����Ϊ2���ֽ�
}__attribute__((packed)) ST_ZJCT_CALL_SEM_HEADER;

#define ZJCT_CALL_SEM_MSG_HEADER_LEN sizeof(ST_ZJCT_CALL_SEM_HEADER)

/* װ�׳�ͨ�豸��������������Ϣ����*/
#define ZJCT_SUBDATA_HEADLEN 	2  //��Ϣ�����ͺ���Ϣ�������ݳ���

typedef struct _zjct_sem_msg_content_
{
	uint8  subType;
	uint8  subLen;
	uint8  subData[ZJCT_MAX_CALLSEM_DATA_LEN];
}__attribute__((packed)) ST_ZJCT_SEM_MSG_CONTENT;

/* װ�׳�ͨ�豸��������������Ϣ��������*/
typedef struct _zjct_call_sem_msg
{	
	ST_ZJCT_CALL_SEM_HEADER Head;
	//uint8 Data[ZJCT_SUBDATA_HEADLEN + ZJCT_MAX_CALLSEM_DATA_LEN]; //130�ֽ�
	ST_ZJCT_SEM_MSG_CONTENT MsgData;
}__attribute__((packed)) ST_CALL_SEM_MSG;

typedef struct _zjct_inccall_voc_msg
{
	ST_ZJCT_CALL_SEM_HEADER Head;
	uint8 inc_vocdata[ZJCT_MAX_CALLSEM_DATA_LEN];
}__attribute__((packed))ZJCT_INCCALL_VOC_MSG;
/* �豸״̬����֡*/
typedef struct _zjct_status_frame
{
	uint8	cfg_status;
	uint8 	dev_srcId;
	uint8	dev_prio;
	uint8	dev_mode;
	uint16	dev_dscId;
	uint16 	hw_ver;
	uint32	sw_ver;
	uint8	user_num[17];
}__attribute__((packed))ZJCT_STATUS_FRAME_DATA;
 /* װ�׳�ͨ��Ա*/
typedef struct _zjct_mem
{
	ZJCT_STATUS_FRAME_DATA data;
	uint32	Ipaddr;				/* network ip addr */
	uint8 	Netmask;			/* network netmask */
	uint8 	online;				/* online flag */
//	uint8 	used;				/* online flag */
	uint32  time_cnt;			/* save the time every 10ms */
//	uint8 	user_name_flg;          /* user phone number register flag */
//	uint8 	user_name_length;	/* user phone number length */
//	uint8	user_name[10];		/* user phone number */
	uint8	user_password[10];	
	uint8 	connect; 			//����
	uint8 	callIn;				//����
	uint8 	callIn_id;			//����	
	uint8   active_release;	//����
	uint8 	force_call_srcId;
	uint8 	radio_ctl_status;	/*radio control*/
	uint8 	radio_gtctl_status;	/*radio gongting control*/
	uint8 	inc_connect;		/*network control device connect status*/
	uint8 	inc_callIn;	
	uint8	group_call_status;
	uint8	group_call_id;
	uint8	mem_save;			/* voice save */
	uint8	mem_get;			/* voice get */
	uint8 	mem_chan_id;			/* ac491 channel id ,0xFF show no chan*/
//	uint8 	user_phone_length;	
//	uint8 	user_phone_num[10];
	uint8 	mem_voc_flg;
	uint8 	mem_voc[VOICE_TIME_1min][ZJCT_VOC_PCM_20MS_LEN_320];  /* remote member voc data*/

	uint8 	glocaldev_num;
}__attribute__((packed))ZJCT_MEM;

typedef enum _radio_ctl_status
{
	RADIO_CTL_DISCONNECT = 0x00,
	RADIO_CTL_CONNECT = 0x01,
}RADIO_CTL_STATUS;

 typedef enum  _work_mode{
	SEAT_MODE = 0,
	CY1HH_MODE = 1,	
}WORK_MODE;

 typedef enum  _ac491work_mode{
	AC491_STD_MODE = 0,
	AC491_ADD_MODE = 1,	
}AC491WORK_MODE;

 typedef struct _BOARD_INC_MEM_INFO
{
	uint8 mem_num;
	uint8 msgack_status;
	uint8 pnum_len;
	uint8 user_num[10];
	uint8 omeeting;
	uint8 prio;
	uint8 encode_type;
	uint8 timeslot;
	uint8 online;
	uint8 usernumreq_status;
	uint8 usernumunreq_status;
}__attribute__((packed)) ZJCT_INC_MEM_INFO;

#undef 	ZJCT_MAX_SEMDATLEN
#define ZJCT_MAX_SEMDATLEN	600
typedef struct _zjct_seat_status_msg
{	
	ST_ZJCT_MNG_HEADER Head;
	uint8 param_cfg;
	uint8 srcId;
	uint8 src_prio;
	uint8 src_mode;
	uint8 desId;
	uint8 callIn;
}__attribute__((packed)) ZJCT_SEAT_STATUS_MSG;

typedef struct _ETH_PORT_
{
	uint32 ipaddr;
	uint32 netmask;
	uint16 seat_sem_port;
	uint16 seat_voc_port;
	uint16 seat_dbg_port;
	uint16 seat_data_port;
	uint16 seat_mng_port;
	uint16 zjct_adptmng_port;
	uint16 zjct_adptvoc_port;
	uint16 zjct_incsem_port;
	uint16 zjct_incvoc_port;
}__attribute__((packed)) ETH_PORT_DES;

#define ZJCT_SEAT_STATUS_LEN sizeof(ZJCT_SEAT_STATUS_MSG)
#define ZJCT_SEAT_STATUS_DATALEN 7

typedef enum _zjct_cmd_type
{
	ZJCT_SEAT_CONFIG = 0x00,
	ZJCT_SEAT_VOICE = 0x01
}ZJCT_CMD_TYPE;

typedef enum _zjct_danhu_cmd_
{
	DANHU_CALLIN = 0x01,				/*���к���*/
	DANHU_ANSWER = 0x02,				/*���н���*/
	DANHU_CALLOUT =0x03,				/*���к���*/
	DANHU_CONNECT = 0x04,				/*��������*/
	DANHU_DISCONNECT_PASSIVE = 0x05,	/*�����һ�*/
	DANHU_DISCONNECT_PASSIVE_ACK = 0x06,/*�����һ�Ӧ��*/
	DANHU_DISCONNECT_ACTIVE = 0x07,		/*�����һ�*/
	DANHU_TIMEOUT = 0x08,				/*�������г�ʱ*/
	DANHU_BUSY = 0x09,					/*����æ*/
	DANHU_CALLIN_REFUSE = 0x0A,			/*���оܾ�*/

}ZJCT_DANHU_CMD;

#define	DANHU_MSG_BYTE	 0x0d

typedef enum _zjct_forcecall_cmd
{
	FORCECALLOUT = 0x01,
	CANCEL_FORCECALLOUT = 0x02,
	FORCECALLIN = 0x03,
	FORCECALLIN_END = 0x04
}ZJCT_FORCECALL_CMD;

typedef enum _zjct_groupcall_cmd
{
	GROUPCALLOUT = 0x01,
	CANCEL_GROUPCALLOUT = 0x02,
	GROUPCALLIN = 0x03,
	CANCEL_GROUPCALLIN = 0x04,
	GROUPCALL_HANDUP = 0x05,
	
}ZJCT_GROUPCALL_CMD;

typedef enum _zjct_setting
{

	ADDR_SETTING = 0x01,
		
}ZJCT_SETTING;

typedef enum _zjct_handle
{
	HANDLE_INIT  = 0x01,
	HANDLE_PC = 0x02
}ZJCT_HANDLE;

typedef enum _zjct_inquire_terminal
{
	INQUIRE_ALL_TERMINAL = 0x01,
	INQUIRE_ALL_TERMINAL_ACK = 0x02,
	INQUIRE_ONE_TERMINAL = 0x03,
	INQUIRE_ONE_TERMINAL_ACK = 0x04
}ZJCT_INQUIRE_TERMINAL;

typedef enum _zjct_inquire_radio
{
	INQUIRE_ALL_RADIO = 0x01,
	INQUIRE_ALL_RADIO_ACK = 0x02,
	INQUIRE_ONE_RADIO = 0x03,
	INQUIRE_ONE_RADIO_ACK = 0x04
}ZJCT_INQUIRE_RADIO;

/* ��̨ת��*/
enum __SEAT_DTZX_MODULE_
{
	SEAT_RADIO_ADDR = 0x01,
	SEAT_STOP,
	SEAT_A_TO_B,
	SEAT_B_TO_A,
	SEAT_AB_AUTO
};
enum _DTZX_MODULE_
{
	DTZX_START = 0x01,
	DTZX_STOP = 0x02
};
enum _DTZX_CTL_TYPE_
{
	DTZX_HANDLE = 0x01,
	DTZX_AUTO,
	DTZX_INTERFACE,
};

enum __DHCP_STATUS__
{
	DHCP_FAILED = 0x01,
	DHCP_OK,
};

enum __DHCP_CFG__
{
	DHCP_NOCFG = 0x00,
	DHCP_CFG,
};

enum __CHAN_CFG__
{
	CHAN_NOCFG = 0x00,
	CHAN_CFG,
};

enum __AC491_CHAN_STATUS__
{
	CHAN_FREE = 0x00,
	CHAN_USING,
};

enum __AC491_VOC_SENDTO__
{
	ZJCT_SENDTO_NULL	= 0x00,
	ZJCT_SENDTO_ADAPTER,				
	ZJCT_SENDTO_INC,						
	ZJCT_SENDTO_RADIO,					
	ZJCT_SENDTO_GROUP,					
};


/*****************************************************************************************************
 * �ص���������
*****************************************************************************************************/

/*****************************************************************************************************
 * �ڲ���������
*****************************************************************************************************/


/*****************************************************************************************************
 * ������������
*****************************************************************************************************/
extern int32 Zjct_init(void);
extern int32 Zjct_Dbg_func_for_710(void);
extern int32 Zjct_Ac491Voc_To_MainBoard(uint16 btimeslot, uint8 *p, uint32 len);
extern int32 Zjct_AdptMng_AutoSendStatusMsg(uint8 id);
extern int32 Zjct_AdptMng_SendRegisterNetwork_Req(uint8 id);
extern int32 Zjct_AdptMng_SendParamConfig_Req(uint8 id);
extern int32 Zjct_AdptMng_Update_MemList_OffLine_Msg(void);
extern int32 Zjct_IncCall_SendOpenUserMsg(void);
extern int32 Zjct_IncCall_Register_UserPhoneNumber(uint8 *buf, uint32 len);
extern int32 Zjct_AdptMng_AutoSend_Open_GroupCall(void);
extern int32 Zjct_Handle_Status_Check(void);
extern int32 Zjct_Handle_Status_Frame_SendTo_PC(void);
extern int32 Zjct_DanHu_Status_TimeOut_Check(void);

extern void Zjct_SeatMng_RxThread(void);
extern void Zjct_AdptMng_RxThread(void);
extern void Zjct_AdptVoc_RxThread(void);
extern void Zjct_SeatCall_RxThread(void);
extern void Zjct_SeatData_RxThread(void);
extern void Zjct_IncCall_RxThread(void);
extern void Zjct_IncVoc_RxThread(void);
extern int32 Zjct_Ptt_Process(uint8 flag, uint8 id);
extern int32 Zjct_SeatMng_Socket_init(void);
extern int32 Zjct_AdptMng_Update_Radio_OffLine_Msg(void);
extern int32 Zjct_AdptVoc_SendTo_AC491_Save(void);
extern int32 Zjct_SeatMng_Socket_Send(uint8 *buf, uint32 len);
extern int32 Zjct_Socket_Close(void);
extern int32 Zjct_AdptVoc_Msg_Process_Add(uint8 *buf, uint32 len);
extern int32 Zjct_AdptMng_Update_Mem2List_OnLine_Msg(uint8 *buf, uint32 len, uint32 Ipaddr );
extern int32 Zjct_IncCall_Socket_Send(uint8 *buf, uint32 len, uint8 id);
extern int32 Zjct_IncVoc_Socket_Send(uint8 *buf, uint32 len, uint8 id);
extern int32 BoardSem_CallAck_busy(uint8 *buf, int32 len, uint8 id);
extern int32 BoardSem_Call_HandUp(uint8 *buf, int32 len, uint8 id);
extern int32 Zjct_AdptVoc_Socket_Send(uint8 *buf, uint32 len);
extern int32 Zjct_Ptt_Send_Inc(uint8 ptt_flg, uint8 id);

extern int Zjct_Mng_Thread(const void *data);

extern void Zjct_AdptMng_0_RxThread(void);
extern void Zjct_AdptMng_1_RxThread(void);
extern void Zjct_AdptMng_2_RxThread(void);
extern void Zjct_AdptMng_3_RxThread(void);
extern void Zjct_AdptMng_4_RxThread(void);
extern void Zjct_AdptMng_5_RxThread(void);
extern void Zjct_AdptMng_6_RxThread(void);
extern void Zjct_AdptMng_7_RxThread(void);
extern void Zjct_AdptMng_8_RxThread(void);
extern void Zjct_AdptMng_9_RxThread(void);
extern void Zjct_AdptMng_10_RxThread(void);
extern void Zjct_AdptMng_11_RxThread(void);


extern void Zjct_IncCall_0_RxThread(void);
extern void Zjct_IncVoc_0_RxThread(void);
extern void Zjct_IncCall_1_RxThread(void);
extern void Zjct_IncVoc_1_RxThread(void);
extern void Zjct_IncCall_2_RxThread(void);
extern void Zjct_IncVoc_2_RxThread(void);
extern void Zjct_IncCall_3_RxThread(void);
extern void Zjct_IncVoc_3_RxThread(void);
extern void Zjct_IncCall_4_RxThread(void);
extern void Zjct_IncVoc_4_RxThread(void);
extern void Zjct_IncCall_5_RxThread(void);
extern void Zjct_IncVoc_5_RxThread(void);
extern void Zjct_IncCall_6_RxThread(void);
extern void Zjct_IncVoc_6_RxThread(void);
extern void Zjct_IncCall_7_RxThread(void);
extern void Zjct_IncVoc_7_RxThread(void);
extern void Zjct_IncCall_8_RxThread(void);
extern void Zjct_IncVoc_8_RxThread(void);
extern void Zjct_IncCall_9_RxThread(void);
extern void Zjct_IncVoc_9_RxThread(void);
extern void Zjct_IncCall_10_RxThread(void);
extern void Zjct_IncVoc_10_RxThread(void);
extern void Zjct_IncCall_11_RxThread(void);
extern void Zjct_IncVoc_11_RxThread(void);


extern uint8 gsend_register[CHETONG_SEAT_MAX];
extern uint8 gnetwork_config ;
extern uint8 grequest_status[CHETONG_SEAT_MAX];
extern uint8 gopenusermsg_status;
extern uint8 gsocketinit_status;
extern uint8 gusernum_status;
extern uint32 gdanhu_timeout_flg;
extern uint8 gauto_send_cfg_flg;
extern uint8 panel_status;
extern ETH_PORT_DES II_Adpt_local[CHETONG_SEAT_MAX];
extern ETH_PORT_DES II_Adpt_remote;

extern ETH_PORT_DES II_Inc_local[CHETONG_SEAT_MAX];
extern ETH_PORT_DES II_Inc_remote;

#if defined(__cplusplus) 
} 
#endif 

#endif // __ZJCT_PROC_H__

