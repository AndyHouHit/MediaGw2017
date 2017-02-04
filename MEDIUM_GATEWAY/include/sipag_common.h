/*
===========================================================================
** file name: sipag_common.h
** description: ��SIP��ͨ��ͷ�ļ�
===========================================================================
*/
#ifndef __SIPAG_COMMON_H__
#define __SIPAG_COMMON_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" { 
#endif


#define		MAXNUMLEN	20/*���������鳤��*/
#define		IDLE		0xee/*��Ч���ݱ�ʾ*/


/*SIP�������õķ���ֵ*/
enum{
	EN_SIP_OK = 0,
	EN_SIP_ERROR = -1,
};


typedef enum{
	PACK_TYPE_RTP = 1,
	PACK_TYPE_RTCP = 2
}EN_SIPAG_PACK_TYPE;

typedef enum{
	SIPAG_CALL_STATUS_ANSWER = 0x01,
	SIPAG_CALL_STATUS_BUSY ,
	SIPAG_CALL_STATUS_CANCEL ,
}EN_SIPAG_CALL_STATUS;

typedef enum{
	SIPAG_PTT_DOWN = 0x01,
	SIPAG_PTT_UP,
	SIPAG_PTT_HOLD,
}EN_SIPAG_PTT_STATUS;

/*1.��������,��Ӧ*/
#if 0
!brief -- sip example
ST_SIP_IPC_MSG_CALL_REQ CallReq;
strcpy(CallReq.CallerNum,"1234567");/*��д���к���*/
strcpy(CallReq.CalleeNum,"7654321");/*��д���к���*/
CallReq.Domain = inet_addr("10.11.12.13");/*��д�Զ�IP��ַ*/
if(st_sipag_intf_func.SIP_Call_Request(&CallReq) == EN_SIP_OK)
	call_id = CallReq.CallID;/*��ȡcallid,��Ϊ������Ϣ�ı�ʶ����*/
#endif

typedef struct{
        unsigned char useType;/*useType��������*/
		char CallerNum[MAXNUMLEN];
		char CalleeNum[MAXNUMLEN];
		unsigned long Domain;/*�Զ�UA��ַ��Ϣ,�����ֽ���*/
		unsigned short CallID;
}ST_SIP_IPC_MSG_CALL_REQ,ST_SIP_IPC_MSG_CALL_EVENT;


/*2.����Ӧ������,��Ӧ*/
typedef struct{
		unsigned short CallID;
		unsigned char useType;
		EN_SIPAG_CALL_STATUS useStatus;
		unsigned long Domain;/*�Զ�UA��ַ��Ϣ,�����ֽ���*/
		char CalleeNum[MAXNUMLEN];
}ST_SIP_IPC_MSG_CALLACK_REQ,ST_SIP_IPC_MSG_CALLACK_EVENT;


/*3.��������,��Ӧ*/
typedef struct{
		unsigned short CallID;
		unsigned char useType;
}ST_SIP_IPC_MSG_CONNECT_REQ,ST_SIP_IPC_MSG_CONNECT_EVENT;

/*4.�ͷ�����,��Ӧ*/
typedef struct{
		unsigned short CallID;
		unsigned char useType;
		EN_SIPAG_CALL_STATUS useStatus;
}ST_SIP_IPC_MSG_RELEASE_REQ,ST_SIP_IPC_MSG_RELEASE_EVENT;

/*5.�����û�*/
typedef struct{
		char name[MAXNUMLEN];/*�û�����*/
}ST_SIP_IPC_MSG_USER_MSG;

typedef struct{
		unsigned short CallID;
		EN_SIPAG_PACK_TYPE packType;
		short packLen;
		char pack[1600];
}ST_SIP_IPC_MSG_PACK_READ,ST_SIP_IPC_MSG_PACK_WRITE;

typedef struct{
	unsigned short CallID;
	EN_SIPAG_PTT_STATUS status;
}ST_SIP_IPC_MSG_PTT_MSG;

typedef struct{
		int (*SIP_Call_Request)(ST_SIP_IPC_MSG_CALL_REQ *);
		int (*SIP_Call_Notify)(ST_SIP_IPC_MSG_CALL_EVENT *);
		
		int (*SIP_CallAck_Request)(ST_SIP_IPC_MSG_CALLACK_REQ *);
		int (*SIP_CallAck_Notify)(ST_SIP_IPC_MSG_CALLACK_EVENT *);
		
		int (*SIP_Connect_Request)(ST_SIP_IPC_MSG_CONNECT_REQ *);
		int (*SIP_Connect_Notify)(ST_SIP_IPC_MSG_CONNECT_EVENT *);
		
		int (*SIP_Release_Request)(ST_SIP_IPC_MSG_RELEASE_REQ *);
		int (*SIP_Release_Notify)(ST_SIP_IPC_MSG_RELEASE_EVENT *);

		int (*SIP_Append_User)(ST_SIP_IPC_MSG_USER_MSG *);

		int (*SIP_Pack_Read)(ST_SIP_IPC_MSG_PACK_READ *);
		int (*SIP_Pack_Write)(ST_SIP_IPC_MSG_PACK_WRITE *);

		int (*SIP_PTT_Request)(ST_SIP_IPC_MSG_PTT_MSG *);
		int (*SIP_PTT_Notify)(ST_SIP_IPC_MSG_PTT_MSG *);
}ST_SIPAG_INTF_FUNC;

extern	ST_SIPAG_INTF_FUNC st_sipag_intf_func;
extern int init_logger(void);
extern int sip_load_module(struct in_addr);

#if defined(__cplusplus) || defined(c_plusplus)
} 
#endif 

#endif
