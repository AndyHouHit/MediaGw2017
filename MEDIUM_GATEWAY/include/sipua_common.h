/*
===========================================================================
** file name: sipua_common.h
** description: ��SIP��ͨ��ͷ�ļ�
===========================================================================
*/
#ifndef __SIPUA_COMMON_H__
#define __SIPUA_COMMON_H__

#if defined(__cplusplus) 
extern "C" 
{ 
#endif




typedef enum
{
	EXT_GETEWAY = 1,
	DIANTAI_GATEWAY,
	G_GATEWAY
} EN_SIP_IPC_GATEWAY_TYPE;

typedef enum  {

	EN_MSG_CALL_REQUEST = 0x01,//����������Ϣ	0x01
	EN_MSG_CALL_NUMBER,//���봫����Ϣ	0x02
	EN_MSG_CALL_ACK,//����Ӧ����Ϣ	0x03
	EN_MSG_CONNECT_REQUEST,//����������Ϣ	0x04
	EN_MSG_RELEASE_REQUEST,//�ͷ�������Ϣ	0x05
	EN_MSG_RELEASE_ACK,//�ͷ�֤ʵ��Ϣ	0x06
	EN_MSG_DELETE_CONFORCE,//ɾ�������Ա������Ϣ	0x07
	EN_MSG_DELETE_CONFORCE_ACK,//ɾ�������Ա֤ʵ��Ϣ	0x08
    EN_MSG_PTT,//PTT������Ϣ	0x09
    EN_MSG_CALL_BUTT
} EN_SIP_IPC_MSG_TYPE;

typedef enum  {
	EN_SEL_CALL = 0x01,//0x01   ѡ��
	EN_PLAN_CONF,//0x02   ԤԼȫ˫������
	EN_RANDOM_CONF,//0x03   ���ȫ˫������
	EN_ADD_CALL_MEMBER,//0x04   ���ӻ����Ա
	EN_GROUP_CALL,//0x05   Ⱥ��
	EN_FORCE_CALL,//0x06   ǿ��
	EN_FORCE_RELEASE_CALL,//0x07   ǿ��
	EN_LISTENING,//0x08   ����
	EN_CALL_FORWARD,//0x09   ����ת��
	EN_CALL_FORWARD_TO,//0x0a   ����ת��
	EN_SPECIFIC_CALL,//0x0b   ר��
	EN_OMIT_CALL,//0x0c   ��λ����

    EN_MSG_USE_TYPE_BUTT
} EN_SIP_IPC_MSG_USE_TYPE;


typedef enum
{
	EN_SPEAK_ONLY = 1,
	EN_LISTEN_ONLY,
	EN_DOUBLE,//˫��
	EN_SILENT //��Ĭ
}EN_SIP_IPC_MSG_MEDIA_TYPE;


typedef enum
{
	EN_ERROR_FORWARD = 1,
	EN_ERROR_BACKWARD
}EN_SIP_IPC_MSG_ERROR_DIR;


/*SIP�������õĳ�����Ϣ��ʶ*/
typedef enum
{
	EN_SIP_OK = 0,
	EN_SIP_ERROR = -1
}EN_SIP_IPC_ERRNO;

/*������*/
typedef enum
{
	PACK_TYPE_RTP = 1,
	PACK_TYPE_RTCP = 2
}EN_SIP_IPC_MSG_PACK_TYPE;

//ǰ������,������Ӧ
/*1.��������,��Ӧ*/
typedef struct _Sip_IPC_Msg_Call{
		EN_SIP_IPC_MSG_TYPE msgSubType;//��Ϣ������,0x01
		unsigned char membPortIndex;//��Ա��Ӧ������˿ں�
        	EN_SIP_IPC_MSG_USE_TYPE useType;//��������
		unsigned char chanType;//�ŵ�����,���ߣ�0x00�����ߣ�0x01
		unsigned char CallerNum[MAXNUMLEN];//���к���
		unsigned char CalleeNum[MAXNUMLEN];//���к���
		unsigned short CallID;//����ID 0--error  >0--����
		unsigned int msgSize;//��Ϣ����
}ST_SIP_IPC_MSG_CALL_REQ,ST_SIP_IPC_MSG_CALL_EVENT;


/*2.����Ӧ������,��Ӧ*/
typedef struct _Sip_IPC_Msg_CallAck{
		EN_SIP_IPC_MSG_TYPE msgSubType;//��Ϣ������,0x02
		unsigned char membPortIndex;//��Ա��Ӧ������˿ں�
        	EN_SIP_IPC_MSG_USE_TYPE useType;//��������
		unsigned char useStatus;//����״̬
		unsigned char msgSize;//��Ϣ����
		unsigned char CallerNum[MAXNUMLEN];//���к���
		unsigned char CalleeNum[MAXNUMLEN];//���к���
		unsigned short CallID;//����ID 0--error  >0--����
}ST_SIP_IPC_MSG_CALLACK_REQ,ST_SIP_IPC_MSG_CALLACK_EVENT;


/*3.��������,��Ӧ*/
typedef struct _Sip_IPC_Msg_Connect{
		EN_SIP_IPC_MSG_TYPE msgSubType;//��Ϣ������
		unsigned char membPortIndex;//��Ա��Ӧ������˿ں�
        	EN_SIP_IPC_MSG_USE_TYPE useType;//��������
		unsigned char chanType;//�ŵ�����,���ߣ�0x00�����ߣ�0x01
		unsigned char CallerNum[MAXNUMLEN];//���к���
		unsigned char CalleeNum[MAXNUMLEN];//���к���
		unsigned short CallID;//����ID 0--error  >0--����
		unsigned char msgSize;//��Ϣ����
}ST_SIP_IPC_MSG_CONNECT_REQ,ST_SIP_IPC_MSG_CONNECT_EVENT;

/*4.�ͷ�����,��Ӧ*/
typedef struct _Sip_IPC_Msg_Release{
		EN_SIP_IPC_MSG_TYPE msgSubType;//��Ϣ������
		unsigned char membPortIndex;//��Ա��Ӧ������˿ں�
        	EN_SIP_IPC_MSG_USE_TYPE useType;//��������
		unsigned char useStatus;//�ͷ�ԭ��
		unsigned char CallerNum[MAXNUMLEN];//���к���
		unsigned char CalleeNum[MAXNUMLEN];//���к���
		unsigned short CallID;//����ID 0--error  >0--����
		unsigned char msgSize;//��Ϣ����
}ST_SIP_IPC_MSG_RELEASE_REQ,ST_SIP_IPC_MSG_RELEASE_EVENT;

/*5.���ӻ����Ա*/
typedef struct _Sip_IPC_Msg_Add_Memb{
		EN_SIP_IPC_MSG_TYPE msgSubType;//��Ϣ������
		unsigned char membPortIndex;//��Ա��Ӧ������˿ں�
		unsigned char AddNumLen;//���ӳ�Ա�ĺ��볤��
		unsigned char AddCalleeNum[MAXNUMLEN];//���ӳ�Ա�ĺ���
		unsigned short CallID;//����ID 0--error  >0--����
		unsigned char msgSize;//��Ϣ����
}ST_SIP_IPC_MSG_ADD_MEMB_REQ,ST_SIP_IPC_MSG_ADD_MEMB_EVENT;

/*6.ɾ�������Ա*/
typedef struct _Sip_IPC_Msg_Del_Memb{
		EN_SIP_IPC_MSG_TYPE msgSubType;//��Ϣ������
		unsigned char membPortIndex;//��Ա��Ӧ������˿ں�
		unsigned char delNumLen;//ɾ����Ա�ĺ��볤��
		unsigned char delCalleeNum[MAXNUMLEN];//ɾ����Ա�ĺ���
		unsigned short CallID;//����ID 0--error  >0--����
		unsigned char msgSize;//��Ϣ����
}ST_SIP_IPC_MSG_DEL_MEMB_REQ,ST_SIP_IPC_MSG_DEL_MEMB_EVENT;

/*7.ý����Э������,��Ӧ*/
typedef struct _Sip_IPC_Msg_Media{
		EN_SIP_IPC_MSG_TYPE msgSubType;
		unsigned char membPortIndex;
		unsigned short CallID;
		EN_SIP_IPC_MSG_MEDIA_TYPE mediaType;
		unsigned char msgSize;
}ST_SIP_IPC_MSG_MEDIA_REQ,ST_SIP_IPC_MSG_MEDIA_EVENT;

/*
** 8.ǰ��,���������Ϣ--->��������첽�����ĵ��ù�����,
** ��ʹ���첽�Ĵ�����Ϣ���ݻ���
*/
typedef struct _Sip_IPC_Msg_Error
{
		EN_SIP_IPC_MSG_TYPE msgSubType;
		unsigned char membPortIndex;
		EN_SIP_IPC_MSG_ERROR_DIR err_direction;/*1--forward--���ذ�->���а� 2--backward--���а�->���ذ�*/
		unsigned short CallID;
		EN_SIP_IPC_ERRNO err_type;
		unsigned char err_info[64];
}ST_SIP_IPC_MSG_ERROR;


/*9.rtp/rtcp���շ�*/
typedef struct _Sip_IPC_Msg_Pack_Trans{
		EN_SIP_IPC_MSG_TYPE msgSubType;//��Ϣ������
		unsigned char membPortIndex;//��Ա��Ӧ������˿ں�
		unsigned char msgSize;//��Ϣ����
		unsigned short CallID;//����ID 0--error  >0--����
		EN_SIP_IPC_MSG_PACK_TYPE packTyep;
		unsigned short packLen;
		char pack[1600];
}ST_SIP_IPC_MSG_PACK_READ,ST_SIP_IPC_MSG_PACK_WRITE;

//�����ӿڼ�
typedef struct _Sip_Ua_intf_Func{
		int (*SIP_Call_Request)(ST_SIP_IPC_MSG_CALL_REQ *Callreq);
		int (*SIP_Call_Notify)(ST_SIP_IPC_MSG_CALL_EVENT* Callevent);
		
		int (*SIP_CallAck_Request)(ST_SIP_IPC_MSG_CALLACK_REQ *CallAckreq);
		int (*SIP_CallAck_Notify)(ST_SIP_IPC_MSG_CALLACK_EVENT* CallAckevent);
		
		
		int (*SIP_Connect_Request)(ST_SIP_IPC_MSG_CONNECT_REQ *Connectreq);
		int (*SIP_Connect_Notify)(ST_SIP_IPC_MSG_CONNECT_EVENT* Connectevent);
		
		
		int (*SIP_Release_Request)(ST_SIP_IPC_MSG_RELEASE_REQ *Rlsreq);
		int (*SIP_Release_Notify)(ST_SIP_IPC_MSG_RELEASE_EVENT* Rlsevent);
		
		int (*SIP_Add_Member_Request)(ST_SIP_IPC_MSG_ADD_MEMB_REQ* AddMembreq);
		int (*SIP_Add_Member_Notify)(ST_SIP_IPC_MSG_ADD_MEMB_EVENT* AddMembevent);
		
		int (*SIP_Del_Member_Request)(ST_SIP_IPC_MSG_DEL_MEMB_REQ *DelMembreq);
		int (*SIP_Del_Member_Notify)(ST_SIP_IPC_MSG_DEL_MEMB_EVENT *DelMembevent);
		
		
		int (*SIP_Media_Request)(ST_SIP_IPC_MSG_MEDIA_REQ* Mediareq);
		int (*SIP_Media_Notify)(ST_SIP_IPC_MSG_MEDIA_EVENT* Mediaevent);
		
		
		int (*SIP_Error_Forward)(ST_SIP_IPC_MSG_ERROR* Msgerr);
		int (*SIP_Error_Backward)(ST_SIP_IPC_MSG_ERROR* Msgerr);

		int (*SIP_Pack_Read)(ST_SIP_IPC_MSG_PACK_READ * MsgPack);
		int (*SIP_Pack_Write)(ST_SIP_IPC_MSG_PACK_WRITE * MsgPack);

		//int (*SIP_Ptt_Request)(unsigned int chan, ST_SIP_IPC_MSG_PTT *ptt);
		
}ST_UA_INTF_FUNC;

//��UA�ṩ�ص�����ע�Ṧ��
extern	ST_UA_INTF_FUNC st_ua_intf_func;
int ast_lctx_register(ST_UA_INTF_FUNC *func);


#if defined(__cplusplus) 
} 
#endif 

#endif // __SIPUA_COMMON_H__