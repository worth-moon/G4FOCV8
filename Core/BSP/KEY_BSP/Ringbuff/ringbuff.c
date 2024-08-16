#include "ringbuff.h"

void RingStr_Init(RingStr_t *p)
{
	p->Head = 0;
	p->Tail = 0;
	memset(p->RingStrPending,0,RINGSTR_MAX_CNT);
}

uint8_t Write_RingStr(RingStr_t *p,uint8_t *pdata,uint8_t len)
{	
		uint8_t status = 0x2;
	
		if(len > RINGSTR_MAX_LEN)
			return 1;
			
    if(0 == p->RingStrPending[p->Tail])
    {
			p->RingStr[p->Tail][0] = len;
			memcpy(p->RingStr[p->Tail]+1,pdata,len);
			p->RingStrPending[p->Tail] = 0x1;
			p->Tail = (p->Tail + 1) % RINGSTR_MAX_CNT;
			status = 0;		
    }
		
		return status;
}

uint8_t Read_RingStrEx(RingStr_t *p,uint8_t *pdata,uint8_t *len)
{
	uint8_t status = 0xFF;	
		
	if(1 == p->RingStrPending[p->Head])
	{
		
		*len = p->RingStr[p->Head][0];
		memcpy(pdata,p->RingStr[p->Head]+1,*len);
		p->RingStrPending[p->Head] = 0x0;
		p->Head = (p->Head + 1) % RINGSTR_MAX_CNT;	
		status = 0;
		
	}	
	return status;
}

uint8_t Read_RingStr(RingStr_t *p,uint8_t *pdata,uint8_t *len)
{
	uint8_t status = 0xFF;	
	
	if(1 == p->RingStrPending[p->Head])
	{
		*len = p->RingStr[p->Head][0];
		memcpy(pdata,p->RingStr[p->Head]+1,*len);
		status = 0;
	}
	
	return status;
}

void ReclaimStr(RingStr_t *p)
{
	p->RingStrPending[p->Head] = 0x0;
	p->Head = (p->Head + 1) % RINGSTR_MAX_CNT;
}

