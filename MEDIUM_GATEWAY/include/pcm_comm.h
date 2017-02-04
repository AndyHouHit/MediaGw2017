/****************************************************************************************************
* Copyright @,  2012-2017,  LCTX Co., Ltd. 
* Filename:     pcm_comm.h  
* Version:      1.00
* Date:         2012-04-20	    
* Description:  128K ����PCMת����G.711 A��
* Modification History: ��ʯ�� 2012-04-20 �½��ļ�

*****************************************************************************************************/
//header: pcm_comm.h  
#ifndef __PCM_COMM_H__
#define __PCM_COMM_H__

#if defined(__cplusplus) 
extern "C" 
{ 
#endif

unsigned char linear2alaw(short pcm_val);
short alaw2linear(unsigned char a_val);  
unsigned char linear2ulaw(short pcm_val);
short ulaw2linear(unsigned char u_val); 
unsigned char alaw2ulaw(unsigned char aval);
unsigned char ulaw2alaw(unsigned char uval); 

/*****************************************************************************************************
 * ������������
*****************************************************************************************************/
extern unsigned int Pcm_Voice_Linear2Alaw(unsigned char *linearBuff, unsigned int BuffLen, unsigned char *aLawBuff);
extern unsigned int Pcm_Voice_Alaw2Linear(unsigned char *aLawBuff, unsigned int BuffLen, unsigned char *linearBuff);

#if defined(__cplusplus) 
} 
#endif 

#endif // __PCM_COMM_H__

