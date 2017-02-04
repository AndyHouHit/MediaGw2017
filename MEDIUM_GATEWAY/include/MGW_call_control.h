#ifndef _MGW_CALL_CONTROL_H_
#define	_MGW_CALL_CONTROL_H_

#if defined(__cplusplus)||defined(c_plusplus)
extern "C"
{
#endif

/*
** call control
*/
//#define         BASE_SLOT                       65
#define         MAXXIWEI                        13
//#define         MAXSIP                          23
//#define       XIWEI_OFFSET            	(0)
#define         LOCALTER_OFFSET         (MAXXIWEI)
//#define         SIP_OFFSET                (LOCALTER_OFFSET )
//#define         USER_NUM_MAX           (SIP_OFFSET + MAXSIP)


/* z043 ��32��ʱ϶���ֹ��� 
Ac491 chan	 	�����ն�����  	Slot
0				����			1
1				�м�1           2
2				�м�2           3
3~14			�Զ�����1~12    4~15
15~30			ϯλ1~16		16~31
31				SIP�ն�			32
*/

/* �͸�50���ú��������û���Ӧ��ʱ϶ͨ����(��50������) ��
ע��:50���ն˱�ʶ��1��ʼ����834���ն˱�ʶҲ��1��ʼ�����Ҫ����˺�
*/
#define 	AC491_TIMESLOT_MAP	1

/* 1������ʱ϶�������ʼʱ϶��ƫ��������834���ն˱�ʶҲ��1��ʼ */
#define		QINWU_OFFSET			0x00								/*00*/
#define		QINWU_NUM_MAX		1
#define     QINWU_BASE_SLOT   	0x01

/* 2���м���·ʱ϶�������ʼʱ϶��ƫ���� */
#define		TRUNK_OFFSET			(QINWU_OFFSET + QINWU_NUM_MAX)		/*01*/
#define		TRUNK_NUM_MAX		2
#define		FEIJI_NUM_MAX		TRUNK_NUM_MAX
#define     TRUNK_BASE_SLOT   	(QINWU_BASE_SLOT + QINWU_NUM_MAX)

/* 12���Զ�����ʱ϶�������ʼʱ϶��ƫ���� */
#define 	PHONE_OFFSET 		(TRUNK_OFFSET + TRUNK_NUM_MAX)		/*03*/
#define 	PHONE_NUM_MAX		12
#define		PHONE_BASE_SLOT   	(TRUNK_BASE_SLOT + TRUNK_NUM_MAX)

/* 16��ϯλʱ϶�������ʼʱ϶��ƫ���� */
#define		XIWEI_OFFSET			(PHONE_OFFSET + PHONE_NUM_MAX)		/*15*/
#define 	XIWEI_NUM_MAX		16
#define     XIWEI_BASE_SLOT   	(PHONE_BASE_SLOT + PHONE_NUM_MAX)

/* 1��SIP�ն�ʱ϶�������ʼʱ϶��ƫ���� */
#define 	SIP_OFFSET				(XIWEI_OFFSET + XIWEI_NUM_MAX)		/*31*/
#define 	SIP_NUM_MAX			1
#define		SIP_BASE_SLOT   		(XIWEI_BASE_SLOT + XIWEI_NUM_MAX)

#define     USER_NUM_MAX        (SIP_OFFSET + SIP_NUM_MAX)			/*32*/

/* 50����ǰͶ��׶�ֻ֧�ֿ�ͨ15���û����� */
#define USER_OPEN_NUM_MAX  (15)								/*15*/

#define ZW_ZHUANXIAN_NUM 6 //K�ڱ��1-4,E1-2: 5-6

/*��չ*/
/* 1������ʱ϶�������ʼʱ϶��ƫ��������834���ն˱�ʶҲ��1��ʼ */
#define	KUOZHAN_QINWU_OFFSET		0x00												/*00*/
#define	KUOZHAN_QINWU_NUM_MAX		1
#define	KUOZHAN_QINWU_BASE_SLOT   	0x01

/* 2���м���·ʱ϶�������ʼʱ϶��ƫ���� */
#define	KUOZHAN_TRUNK_OFFSET		(KUOZHAN_QINWU_OFFSET + KUOZHAN_QINWU_NUM_MAX)		/*01*/
#define	KUOZHAN_TRUNK_NUM_MAX		2
#define	KUOZHAN_FEIJI_NUM_MAX		KUOZHAN_TRUNK_NUM_MAX
#define	KUOZHAN_TRUNK_BASE_SLOT   	(KUOZHAN_QINWU_BASE_SLOT + KUOZHAN_QINWU_NUM_MAX)   /*02*/

/* 12���Զ�����ʱ϶�������ʼʱ϶��ƫ���� */
#define KUOZHAN_PHONE_OFFSET 		(KUOZHAN_TRUNK_OFFSET + KUOZHAN_TRUNK_NUM_MAX)		/*03*/
#define KUOZHAN_PHONE_NUM_MAX		12
#define	KUOZHAN_PHONE_BASE_SLOT   	(KUOZHAN_TRUNK_BASE_SLOT + KUOZHAN_TRUNK_NUM_MAX)   /*04*/

/* 12��ϯλʱ϶�������ʼʱ϶��ƫ���� */
#define	KUOZHAN_XIWEI_OFFSET			(KUOZHAN_PHONE_OFFSET + KUOZHAN_PHONE_NUM_MAX)  /*15*/
#define KUOZHAN_XIWEI_NUM_MAX		15
#define KUOZHAN_XIWEI_BASE_SLOT   	(KUOZHAN_PHONE_BASE_SLOT + KUOZHAN_PHONE_NUM_MAX)   /*16*/

/* 10��SIP�ն�ʱ϶�������ʼʱ϶��ƫ���� */
#define KUOZHAN_ADATPER_OFFSET		(KUOZHAN_XIWEI_OFFSET + KUOZHAN_XIWEI_NUM_MAX)		/*30*/
#define KUOZHAN_ADATPER_NUM_MAX	10
#define	KUOZHAN_ADATPER_BASE_SLOT   	(KUOZHAN_XIWEI_BASE_SLOT + KUOZHAN_XIWEI_NUM_MAX) /*31*/

#define KUOZHAN_USER_NUM_MAX		(40)	
#define	KUOZHAN_FENJI_SLOT_OFFSET	(4)

#define	KUOZHAN_CONF_NUM_MAX		(5)
#define	KUOZHAN_CONF_MEM_MAX		(40)

#define	KUOZHAN_CONF_MEM_MAX_USE	(6)

enum
{
	MC_IDLE = 0xee,
	MC_READY = 0,
	MC_WAIT,
	MC_PROCEED,
	MC_RT_TONE,	/*�����յ����к���Ӧ��*/
	MC_TALK,
	MC_HANG,
	MC_RING,
	MC_RINGING,	/* ���з�������Ӧ���״̬*/
	MC_BUSY,	
	MC_REJECT,	/* ����ʧ�ܺ�״̬*/
	MC_CANCEL,
	MC_DIAL,
	MC_USETRK,
	MC_TRANS,		/*����ת��*/
	MC_LISTEN,
	MC_HEAR,
	MC_INACTIVE,
	MC_ACTIVE,
	MC_HANG_ACTIVE, /* ���������һ�*/
	MC_NONUM,	
	MC_NUMERROR,
	MC_TRANSPHONE,		
};

typedef enum HOOK_STATE
{
	HOOK_OFF = 0,
	HOOK_ON
}HOOK_STATE;

typedef enum TONE_TYPE
{
	TONE_DIAL = 0,
	TONE_SILENT,
	TONE_BUSY,
	TONE_CNF,
	TONE_RING,
	TONE_HANG,
	TONE_STOP
}TONE_TYPE;

enum
{
	PORT_TYPE_TRK = 0x01,
	PORT_TYPE_CISHI,
	PORT_TYPE_QINWU,
	PORT_TYPE_YANGSHENG,
	PORT_TYPE_XIWEI,
	PORT_TYPE_SIP,
	PORT_TYPE_PHONE,
	PORT_TYPE_LOCAL_XIWEI,	
	PORT_TYPE_HEADSET,	
	PORT_TYPE_SW_PHONE,		

	PORT_TYPE_KUOZHAN_PHONE,
	PORT_TYPE_KUOZHAN_XIWEI,	
	PORT_TYPE_ADP_PHONE,
	PORT_TYPE_RESV
};

enum{
	MGW_PTT_INIT = 0x00,
	MGW_PTT_DOWN,
	MGW_PTT_UP,
	MGW_PTT_HOLD,
};

enum{
	MGW_RELEASE_REASON_NORMAL = 0x01,
	MGW_RELEASE_REASON_DEVICEFAULT,
	MGW_RELEASE_REASON_TIMEOUT,
};

enum
{
	MGW_TRK_INIT_STATE = 0x20,
	MGW_TRK_IDLE_STATE,
	MGW_TRK_IN_DIAL_STATE,
	MGW_TRK_IN_RINGTONG_STATE,
	MGW_TRK_IN_TALK_STATE,
	MGW_TRK_OUT_DIAL_STATE,
	MGW_TRK_OUT_RINGTONE_STATE,
	MGW_TRK_OUT_TALK_STATE,
};

enum
{
	MGW_CISHI_INIT_STATE = 0x40,
	MGW_CISHI_IDLE_STATE,
	MGW_CISHI_DIAL_STATE,
	MGW_CISHI_RINGTONE_STATE,
	MGW_CISHI_TALK_STATE,
	MGW_CISHI_RING_STATE,
};

enum
{
	MGW_XIWEI_INIT_STATE = 0x60,
	MGW_XIWEI_IDLE_STATE,
	MGW_XIWEI_DIAL_STATE,
	MGW_XIWEI_RINGTONE_STATE,
	MGW_XIWEI_TALK_STATE,
	MGW_XIWEI_RING_STATE
};

enum{
	MGW_SIP_INIT_STATE = 0x80,
	MGW_SIP_IDLE_STATE,
	MGW_SIP_DIAL_STATE,
	MGW_SIP_RINGTONE_STATE,
	MGW_SIP_TALK_STATE,
	MGW_SIP_RING_STATE,
};

enum{
	MGW_PHONE_INIT_STATE = 0xA0,
	MGW_PHONE_IDLE_STATE,
	MGW_PHONE_DIAL_STATE,
	MGW_PHONE_RINGTONE_STATE,
	MGW_PHONE_TALK_STATE,
	MGW_PHONE_RING_STATE,
};

enum
{
	MGW_LOCAL_XIWEI_INIT_STATE = 0xC0,
	MGW_LOCAL_XIWEI_IDLE_STATE,
	MGW_LOCAL_XIWEI_DIAL_STATE,
	MGW_LOCAL_XIWEI_RINGTONE_STATE,
	MGW_LOCAL_XIWEI_TALK_STATE,
	MGW_LOCAL_XIWEI_RING_STATE
};

enum
{
	MGW_HEADSET_INIT_STATE = 0xD0,
	MGW_HEADSET_IDLE_STATE,
	MGW_HEADSET_DIAL_STATE,
	MGW_HEADSET_RINGTONE_STATE,
	MGW_HEADSET_TALK_STATE,
	MGW_HEADSET_RING_STATE
};


enum
{
	MGW_SW_PHONE_INIT_STATE = 0xE0,
	MGW_SW_PHONE_IDLE_STATE,
	MGW_SW_PHONE_DIAL_STATE,
	MGW_SW_PHONE_RINGTONE_STATE,
	MGW_SW_PHONE_TALK_STATE,
	MGW_SW_PHONE_RING_STATE
};

enum
{
	MGW_ADP_PHONE_INIT_STATE = 0xF0,
	MGW_ADP_PHONE_IDLE_STATE,
	MGW_ADP_PHONE_DIAL_STATE,
	MGW_ADP_PHONE_RINGTONE_STATE,
	MGW_ADP_PHONE_TALK_STATE,
	MGW_ADP_PHONE_RING_STATE
};


typedef struct
{
	unsigned char		bFinish;						/*�û������Ƿ��շ��������*/
	unsigned char		bPhoneQueue[MAXPHONENUM];	/*�û��������*/
	unsigned char		bPhoneQueueHead;
	unsigned char		bPhoneQueueTail;
}PHONE_QUEUE;


enum __ZHUANXIAN__STATUS__
{
	ZHUANXIAN_IDLE = 0x00,
	ZHUANXIAN_INIT = 0x01,
	ZHUANXIAN_FREE = 0x02,
	ZHUANXIAN_CALLIN = 0x03,
	ZHUANXIAN_CALLOUT = 0x04,
	ZHUANXIAN_CALLTALK = 0x05,

	ZHUANXIAN_STATUS_BUFF,
};

typedef struct
{
	BYTE	bHookStatus; 	/*�û�ժ�һ�״̬*/
	WORD	wPort;			/*ȫ�ֱ����±����������ػ���ʱ϶*/
	BYTE	bPortType;		/*�û�����: ϯλ,�м�,����,����,SIP�û�*/
	BYTE	bCommand;		/*�û�����״̬*/
	BYTE	bPortState; 		/*�û�״̬����״̬��*/
	BYTE	bPortState1;		/*�û�״̬���ֽ׶�*/
	WORD	wPortDelay; 		/*�û��˿���ʱ*/
	PHONE_QUEUE phone_queue_g;	/*�պŶ���*/
	PHONE_QUEUE phone_queue_s;	/*���Ŷ���*/
	WORD	wConnectPort;		/*���ӵ��±�����*/
	BYTE	bToneType;	/*�ź�������*/
	BYTE	bRingType;	/*��������*/
	BYTE	bCallType;	/*��������*/
	char		name[80];			/*�û����غ���*/
	BYTE	bslot_us;		/*��Ӧ����������ʱ϶*/		
	BYTE	bPTTStatus;		 /*PTT����״̬*/
	UDP_SOCKET	rtp_socket;	/*ϯλ�û�����RTP����*/
	int *	rtp_io_id;		/*����io���������ڱ�ʾio����ɾ����ʹ��*/
	unsigned short call_id;
	unsigned char ans;
	unsigned char	user_num_len;
	unsigned char	user_num[10];
	unsigned char	conference_enable;		/*����Ȩ��*/
	unsigned char	priority;			/*���ȼ�*/
	unsigned char encoder;
	unsigned char rel_reason;		/*�ͷ�ԭ��*/
	int			sched_assign_id;	/*�û�����������Ϣ���þ��*/
	unsigned char bresv;
	unsigned	char	trk_ring; 
	int 	zhuanxian_type;
	int 	zhuanxian_flg;
	int 	zhuanxian_swphone_voc_send;
	int 	pre_zhuanxian_flg;
	int 	zhuanxian_num[15];
	int 	zhuanxian_status;
	int 	zhuanxian_romote_ppt;
	unsigned char buzzer_status;
	uint32 	ipaddr;
	uint8 	sw_phone_online;
	uint8 	sw_phone_flg;
	uint8 	sw_phone_834_flg;
	uint32 	timecnt_100ms;
	uint8 	talk_flg;		/*Sw phone zhuan xian bao wen lv diao */
	uint8 	busy_flg;
	uint8 	connect_port;	
	uint8 	ptt_status;
	uint8 	ptt_status_save;
	
	uint8 	adp_phone_online;
	uint8 	member_num;
	uint8 	fenji;

	uint8 	phone_num_trans_flg;
	uint8 	conf_id;
	uint8 	connect_port_cnt;
	uint8 	connect_port_array[KUOZHAN_CONF_MEM_MAX];	
	uint8 	connect_port_flg[KUOZHAN_CONF_MEM_MAX];
	uint8 	zhenru_slot;
}TERMINAL_BASE;


typedef struct __PARA_MANAGE_//ϵͳ��������
{	
	uint32 	Media_Id32;//ý�����ذ��ַ
	uint16	B_Dcounter;//�������ͨ��������Ϣ��ʱ 10Sһ��
	uint16 	B_Scounter;//�������ͨ��������Ϣ��ʱ 5Sһ��
	uint8 	Flg_Status;//����״̬	
	uint8 	g_SeamStatus;//״̬
	uint8 	Flg_SeamMeetInto;//��Ϣ�����·���ϯλ��־
	uint8 	workmode;/* I   �͹���ģʽ*/
	uint8 	workmode_save;
	uint8 	workmode_update_flg;
	uint8 	bridge;
	uint8 	gun_mode;
	uint8 	gun_link_mode;	
	uint8 	gun_num_max;
	uint8 	gun_single_id;

	uint8 byWarExist;
	uint32 war_timecnt;
	uint16 seat_cnt_max;
	uint8  single_ppt_status;
	
	uint8  adapter_single_online;
	uint8  adapter_single_online_cnt;
	uint32 adapter_single_timecnt;	
	uint8  adapter_gun_online;
	uint32 adapter_gun_timecnt;
	uint8 adapter_834_online;
	uint32 adapter_834_timecnt;
	uint8 single_seat_online;
	uint32 single_seat_timecnt;	
	uint8 single_node_online;
	uint32 single_node_timecnt;	
	uint8  adapter_710_online;
	uint32 adapter_710_timecnt;	
	uint8  adapter_50_online;
	uint32 adapter_50_timecnt;
	uint8 mng_soft_online;
	uint32 mng_soft_timecnt;
	uint8 call_soft_online;
	uint32 call_soft_timecnt;
	uint8 mem_710_online;
	uint8 mem_710_online_cnt;	
	uint32 mem_710_timecnt;
	uint8 ac491_online;
	uint8 ac491_init;
	uint32 ac491_timecnt;
	
	uint8 adapter_led_cnt;
	uint8 timeslot_gun_ipflg;
}__attribute__((packed)) ST_PARA_MANAGE;


typedef struct __Local_Param__
{
	uint8 workmode;
	uint32 Media_Id32;
	uint32 timecnt_100ms;
	uint8 ip_addr_default_flg;
	uint8 ac491_init;
	uint8 control_online;
	uint32 control_timecnt;
	uint8 num_len;
	uint8 cfg_wr_flg;
}Local_Param;

enum{
	FENJI_UNSET = 0x00,
	FENJI_SET,
};

typedef struct{
	char	name[10];
	unsigned char	set_state;
	unsigned char 	use;
}FENJI_BASE;

extern int g_mgwdrv;
extern TERMINAL_BASE	terminal_group[KUOZHAN_USER_NUM_MAX];
extern struct sched_context * con;
extern FENJI_BASE		fenji_group[FEIJI_NUM_MAX];
extern Local_Param Param_update;

typedef struct
{
	unsigned char terminalId;//�������ע��������������ն�ID
	unsigned char creatStatus;//�������̵�״̬
	char phonenum[10];//K��ר�ߺ���
	char jtjnum[2];//0x09, 0x01
}NETWARE_ZHUANXIAN_BASE;

enum __NetWare_ZxCreate_Status__
{
	NETWARE_ZXCREATE_IDLE = 0x00,
	NETWARE_ZXCREATE_REGREQ = 0x01,
	NETWARE_ZXCREATE_REGACK = 0x02,
	NETWARE_ZXCREATE_UNREGREQ = 0x03,
	NETWARE_ZXCREATE_UNREGACK = 0x04,

	NETWARE_ZXCREATE_QUERYACK = 0x05,
	
	NETWARE_ZXCREATE_BUFF
};

enum __NetWare_ZxCreate_Flg__
{
	NETWARE_ZXCREATE_NOTOK = 0x00,
	NETWARE_ZXCREATE_OK = 0x01,
};


#define SET_ZX_CREATE_TERMINALID(kid, tid) 	zhuanxian_group[(kid)].terminalId = (tid)
#define GET_ZX_CREATE_TERMINALID(kid) 		zhuanxian_group[(kid)].terminalId

#define TERMINAL_ID_INVALID 0xFF

extern char jtjno[2];
extern NETWARE_ZHUANXIAN_BASE zhuanxian_group[ZW_ZHUANXIAN_NUM];


int	init_terminal(const void *data);
int	terminal_call_control(const void *data);

int	sw2st_bus(unsigned char ch);
int sw2ac491(unsigned char ch);


int mgw_ext_call_notify(TERMINAL_BASE * );
int mgw_ext_callack_notify(TERMINAL_BASE * );
int mgw_ext_connect_notify(TERMINAL_BASE * );
int mgw_ext_release_notify(TERMINAL_BASE * );


int mgw_scu_call_request(TERMINAL_BASE * );
int mgw_scu_callack_request(TERMINAL_BASE * );
int mgw_scu_connect_request(TERMINAL_BASE * );
int mgw_scu_release_request(TERMINAL_BASE * );
int mgw_scu_release_cnf_request(TERMINAL_BASE * );
int mgw_scu_ptt_request(TERMINAL_BASE *);
int mgw_scu_zhuanxian_callack(TERMINAL_BASE* terext);
int mgw_scu_zhuanxian_connect_request(TERMINAL_BASE* terext);
int mgw_scu_zhuanxian_msg(TERMINAL_BASE * terext);

int mgw_local_seat_call_request(TERMINAL_BASE * );
int mgw_local_seat_callack_request(TERMINAL_BASE * );
int mgw_local_seat_connect_request(TERMINAL_BASE * );
int mgw_local_seat_release_request(TERMINAL_BASE * );
int mgw_local_seat_release_cnf_request(TERMINAL_BASE * );
int mgw_local_seat_ptt_request(TERMINAL_BASE *);







int mgw_sip_call_request(TERMINAL_BASE* );
int mgw_sip_callack_request(TERMINAL_BASE* );
int mgw_sip_connect_request(TERMINAL_BASE* );
int mgw_sip_release_request(TERMINAL_BASE* );
int init_mgw_sip(const void * data);


int close_channel(const void * data);



int	append_phone_number(BYTE ch,BYTE number);
int detect_tone(BYTE ch,BYTE type);

extern void	Send_TONE(WORD ,TONE_TYPE );
extern void	Send_DTMF(WORD ,char );

int do_sendtone(cmd_tbl_t *cmdtp, int argc, char *argv[]);
int do_senddtmf(cmd_tbl_t *cmdtp, int argc, char *argv[]);
int do_trk_action(cmd_tbl_t *cmdtp, int argc, char *argv[]);
int do_listen_slot(cmd_tbl_t * cmdtp,int argc,char * argv [ ]);
int do_hear_slot(cmd_tbl_t * cmdtp,int argc,char * argv [ ]);
//void *do_sip_rtp_monitor(void *);
int find_port_by_name(const char * name);
int find_port_by_name_sip(const char * name);
int find_port_by_CallID(unsigned short );
int find_port_by_simple_ip(uint32 ipaddr);
int find_port_by_simple_slot(unsigned char slot);
int get_port_sip_idle(void);
int send_dtmf_string(const void * data);


int	use_trk(TERMINAL_BASE* trkext);
int free_trk(TERMINAL_BASE* trkext);
int ring_cishi_off(TERMINAL_BASE* cishiext);
int ring_cishi_on(TERMINAL_BASE* cishiext);
int update_hookstatus(void);
int parse_phonenum(TERMINAL_BASE * ext);

void	MGW_TRK_INIT_Process(TERMINAL_BASE* trkext);
void	MGW_TRK_IDLE_Process(TERMINAL_BASE* trkext);
void	MGW_TRK_IN_RINGTONG_Process(TERMINAL_BASE* trkext);
void	MGW_TRK_IN_TALK_Process(TERMINAL_BASE* trkext);
void	MGW_TRK_IN_DIAL_Process(TERMINAL_BASE* trkext);
void	MGW_TRK_OUT_RINGTONE_Process(TERMINAL_BASE* trkext);
void	MGW_TRK_OUT_DIAL_Process(TERMINAL_BASE* trkext);
void	MGW_TRK_OUT_TALK_Process(TERMINAL_BASE* trkext);

void	MGW_CISHI_INIT_Process(TERMINAL_BASE* cishiext);
void	MGW_CISHI_IDLE_Process(TERMINAL_BASE* cishiext);
void	MGW_CISHI_DIAL_Process(TERMINAL_BASE* cishiext);
void	MGW_CISHI_RINGTONE_Process(TERMINAL_BASE* cishiext);
void	MGW_CISHI_RING_Process(TERMINAL_BASE* cishiext);
void	MGW_CISHI_TALK_Process(TERMINAL_BASE* cishiext);

void	MGW_Phone_INIT_Process(TERMINAL_BASE* phoneext);
void	MGW_Phone_IDLE_Process(TERMINAL_BASE* phoneext);
void	MGW_Phone_DIAL_Process(TERMINAL_BASE* phoneext);
void	MGW_Phone_RINGTONE_Process(TERMINAL_BASE* phoneext);
void	MGW_Phone_RING_Process(TERMINAL_BASE* phoneext);
void	MGW_Phone_TALK_Process(TERMINAL_BASE* phoneext);

void	MGW_XIWEI_INIT_Process(TERMINAL_BASE* xiweiext);
void	MGW_XIWEI_IDLE_Process(TERMINAL_BASE* xiweiext);
void	MGW_XIWEI_DIAL_Process(TERMINAL_BASE* xiweiext);
void	MGW_XIWEI_RINGTONE_Process(TERMINAL_BASE* xiweiext);
void	MGW_XIWEI_RING_Process(TERMINAL_BASE* xiweiext);
void	MGW_XIWEI_TALK_Process(TERMINAL_BASE* xiweiext);

void	MGW_LOCAL_XIWEI_INIT_Process(TERMINAL_BASE* phoneext);
void	MGW_LOCAL_XIWEI_IDLE_Process(TERMINAL_BASE* phoneext);
void	MGW_LOCAL_XIWEI_DIAL_Process(TERMINAL_BASE* phoneext);
void	MGW_LOCAL_XIWEI_RINGTONE_Process(TERMINAL_BASE* phoneext);
void	MGW_LOCAL_XIWEI_RING_Process(TERMINAL_BASE* phoneext);
void	MGW_LOCAL_XIWEI_TALK_Process(TERMINAL_BASE* phoneext);

void	MGW_SIP_INIT_Process(TERMINAL_BASE * sipext);
void	MGW_SIP_IDLE_Process(TERMINAL_BASE * sipext);
void	MGW_SIP_RINGTONE_Process(TERMINAL_BASE * sipext);
void	MGW_SIP_TALK_Process(TERMINAL_BASE * sipext);
void	MGW_SIP_RING_Process(TERMINAL_BASE * sipext);
void	MGW_SIP_DIAL_Process(TERMINAL_BASE * sipext);

int find_js_node_by_callee(const char* num,const char* num1);
//void mgw_broadcast_sip_call_request(struct JS_NODE * node,ST_SIP_IPC_MSG_CALL_REQ* pcallreq);
char* get_fenji(void);
extern int32 Ac491_Init(void);


#if defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /*_MGW_CALL_CONTROL_H_*/

