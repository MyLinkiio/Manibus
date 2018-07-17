#include "musmart_cho.h"
#include "mpdu.h"
#include "i2c.h"
#include "musmart.h"
#include "esp8266.h"

#define MAX_PARAM   10
#define DATA_GET_LIMIT 2

LoopListStruct DataBuffer;

uint8_t pcOrder_data[MAX_DATA_TRANSFORM];      //write data through QueueBus


void Bytes_BUS_RecInit(void){
	
	LoopList_Init(&DataBuffer,RecDataBufferMaxSize);
	
}

void Msg_Filtrate(void){             //���߳�ִ�к���
	
    unsigned char Tmp;
	  unsigned char dir;
	
if(DataBuffer.DataSize>DATA_GET_LIMIT){
	
	  LoopList_GetOneData(&DataBuffer,&Tmp);
		
		if(Tmp>>4==0x0F){
		  
			dir = Tmp&0x0F;
			
			switch(dir){
				
				case 0x0E/*IO*/ :			    
        	cho_io();				
					break;
				
				case 0x0A/*USART*/:
					  cho_usart();			
				break;
				
				
				case 0x0C/*ADC*/:
					  cho_adc();
				break;
				
				
				case 0x0D /*PWM*/:
					  cho_pwm();
					break;
				
				case 0x0B /*IIC*/:
					  cho_iic();
				break;	
				
        case 0x09/*Wifi*/:
        		cho_wifi();
         break;				
			 }
	  	}else{
		return;
		}
   }
}

void msg_feedback(uint8_t byte){
	
	  if(USART1_Channel_FLAG){
			
			//Usart_ReadArray_(&byte,1);
			Usart_SendByte_(USART1,byte);  //��ʱ��ʹ��DMA����
	     USART1_Channel_FLAG = 0x00;   //����ŵ���β��0
	   }
		
	  if(WIFI_Channel_FLAG){
			
			pWifiRead_(&byte,1);
			WIFI_Channel_FLAG = 0x00;
		}
}


u32 read_addr(u32 addr)
{
	return *(u32*)addr;
}

void write_addr(u32 addr,u32 val)
{
	*(u32*)addr=val; 	
}

bool ParamsCheckOut(unsigned short sum,unsigned short count,unsigned char compare){   //����ֵУ�麯��

	  unsigned char temp ;
	  temp = sum % count;
	  if (compare == temp ) return true; else  return false;
	  
}



void cho_io(void){      //��ȷ��
	  
    unsigned char Tmp;
	  unsigned char Tmp1;
	  unsigned char dir;	
	  uint32_t u32_param[MAX_PARAM];

	  unsigned short paramsCheck =0;  
	  unsigned short paramsCount = 0;
	  
	  LoopList_ReadFeature io_feature;
	  
	
  	LoopList_GetOneData(&DataBuffer,&dir);
	
	  io_feature = LoopList_Marked(&DataBuffer);    //����Զ�ȡ����б��
	
	  LoopList_GetOneData(&DataBuffer,&Tmp);  //����õ��˿�
	  u32_param[0] = APB2PERIPH_BASE|(Tmp<<8);
	
	  paramsCheck += Tmp;  paramsCount ++;

	  switch(dir){
			
			case 0xDA:
				
		  LoopList_GetOneData(&DataBuffer,&Tmp);
			 Tmp1 = Tmp>>4;
			 u32_param[1]= (Tmp&0x0F)<<Tmp1;	
			
			paramsCheck += Tmp;  paramsCount ++;
			
			LoopList_GetOneData(&DataBuffer,&Tmp);	
			 u32_param[2] = Tmp;
			
			paramsCheck += Tmp;  paramsCount ++;
			
			if(u32_param[2] != 0x00 && u32_param[2] != 0x01) return;
			

			LoopList_GetOneData(&DataBuffer,&Tmp);			            //������в���ֵУ��
			if(!ParamsCheckOut(paramsCheck,paramsCount,Tmp)) return; 
			
			
		 	LoopList_GetOneData(&DataBuffer,&Tmp);	
			 if(Tmp!= pPinOutPut__ID ){
				 
				 LoopList_FeedBack(&DataBuffer,io_feature.ReadPoint,io_feature.DataSize);  //������󣬻ع鵽���б�ǵ�
			    return;
			 }	
			 
			 (*(u32(*)())usmart_nametab[4].func)(u32_param[0],u32_param[1],u32_param[2]);			 
			break;
			 
			case 0xDB:
				
		  LoopList_GetOneData(&DataBuffer,&Tmp);
			 Tmp1 = Tmp>>4;
			 u32_param[1]= (Tmp&0x0F)<<Tmp1;		
			paramsCheck += Tmp;  paramsCount ++;
			
			LoopList_GetOneData(&DataBuffer,&Tmp);			            //������в���ֵУ��
			if(!ParamsCheckOut(paramsCheck,paramsCount,Tmp)) return; 
			
			LoopList_GetOneData(&DataBuffer,&Tmp); 
		  if(Tmp!= pPinReadBit__ID ){
				LoopList_FeedBack(&DataBuffer,io_feature.ReadPoint,io_feature.DataSize);  //������󣬻ع鵽���б�ǵ�
			    return;
			 }	
			 
			 (*(u32(*)())usmart_nametab[20].func)(u32_param[0],u32_param[1]); 
			break; 
			 
			case 0xCA:
				
		  LoopList_GetOneData(&DataBuffer,&Tmp);
			 Tmp1 = Tmp>>4;
			 u32_param[1]= (Tmp&0x0F)<<Tmp1;
			paramsCheck += Tmp;  paramsCount ++;			
			LoopList_GetOneData(&DataBuffer,&Tmp);
			 u32_param[2] = Tmp;
			paramsCheck += Tmp;  paramsCount ++;
			LoopList_GetOneData(&DataBuffer,&Tmp);
			 u32_param[3] = Tmp;
			paramsCheck += Tmp;  paramsCount ++;
			
			LoopList_GetOneData(&DataBuffer,&Tmp);			            //������в���ֵУ��
			if(!ParamsCheckOut(paramsCheck,paramsCount,Tmp)) return; 
			
		  LoopList_GetOneData(&DataBuffer,&Tmp);	
			 if(Tmp!= pPinInit__ID ){
				 LoopList_FeedBack(&DataBuffer,io_feature.ReadPoint,io_feature.DataSize);  //������󣬻ع鵽���б�ǵ�
			    return;
			 }
			 
			(*(u32(*)())usmart_nametab[2].func)(u32_param[0],(uint16_t)u32_param[1],u32_param[2],u32_param[3]);	
			
			break;
			
			case 0xAA:
				
			LoopList_GetOneData(&DataBuffer,&Tmp);			            //������в���ֵУ��
			if(!ParamsCheckOut(paramsCheck,paramsCount,Tmp)) return; 
			
			 LoopList_GetOneData(&DataBuffer,&Tmp);
			 if(Tmp!= pPinDeInit__ID ){
				 LoopList_FeedBack(&DataBuffer,io_feature.ReadPoint,io_feature.DataSize);  //������󣬻ع鵽���б�ǵ�
			    return;
			 }
			 
			 (*(u32(*)())usmart_nametab[3].func)(u32_param[0]);
			 
		    break;
			 
			 default:	
				  return;

		}
		
		  msg_feedback(IO_Check);    //��βУ��λ
		
}


void s_To_h(const short *val,char hex[2]){    
	
    const char *p =(char*)(val);
    hex[0]=*((char * )p);
    hex[1]=*((char * )(p+1));
}

unsigned int byteMsgSum(unsigned char *msg,unsigned short length){
	
    unsigned short i =0;
  	unsigned int sum = 0;
	
		for(i = length-1;i--;){
		
			sum += msg[i];
		}
     return sum;
}

void cho_usart(void){      //����ʹ��USART2����UART4
	
	
    unsigned char Tmp;
  	unsigned char dir;
	  uint32_t u32_param[MAX_PARAM];
	  short i=0;
	  char datalength =0;
	  char shortParam[2]={0};
		char shortParam_check[3] = {0};
	  unsigned char usart_param_check[2];
		
		unsigned short paramsCheck =0;  
	  unsigned short paramsCount = 0;
		LoopList_ReadFeature io_feature;
		
		
	  LoopList_GetOneData(&DataBuffer,&dir);
	
		
		if(dir == 0xDC){ //����Ϊ��ȡ���ڷ�ʽ���ú���  ����
			
		LoopList_GetOneData(&DataBuffer,&Tmp);
			
		if(Tmp!= pUsartOnLine__ID ){
			
			 return;
		}	
			(*(u32(*)())usmart_nametab[37].func)();
				return;      //���ﲻ�ڵ������з���ֵ
		}
		
		
    LoopList_GetOneData(&DataBuffer,&Tmp); //  Channel 
		u32_param[0] = APB1PERIPH_BASE|(Tmp<<8);
		paramsCheck += Tmp;  paramsCount ++;
		
	  switch(dir){
	
			case 0xDA:
			
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  datalength = Tmp;
			  paramsCheck += Tmp;  paramsCount ++;	
			  
				LoopList_GetOneData(&DataBuffer,&Tmp);			            //������в���ֵУ��
			  if(!ParamsCheckOut(paramsCheck,paramsCount,Tmp)) return;
			  paramsCheck =0;paramsCount=0;
			
			  for(i=0;i<datalength;i++){
					LoopList_GetOneData(&DataBuffer,&Tmp);
					pcOrder_data[i] = Tmp;	
					paramsCheck += Tmp;  paramsCount ++;
				}

				 LoopList_GetOneData(&DataBuffer,&Tmp);			            //������в���ֵУ��
			   if(!ParamsCheckOut(paramsCheck,paramsCount,Tmp)) return;
				
				 LoopList_GetOneData(&DataBuffer,&Tmp);		
			   if(Tmp!= pUsartWrite__ID ){
					 LoopList_FeedBack(&DataBuffer,io_feature.ReadPoint,io_feature.DataSize);  //������󣬻ع鵽���б�ǵ�
			    return;
			   }
				 
				(*(u32(*)())usmart_nametab[7].func)(u32_param[0],pcOrder_data,datalength);			 
			
				break;
				 
			case 0xDB:
				
				 LoopList_GetOneData(&DataBuffer,&Tmp);			            //������в���ֵУ��
			   if(!ParamsCheckOut(paramsCheck,paramsCount,Tmp)) return;
			
				 LoopList_GetOneData(&DataBuffer,&Tmp);	
			   if(Tmp!= pUsartRead__ID ){
					 LoopList_FeedBack(&DataBuffer,io_feature.ReadPoint,io_feature.DataSize);  //������󣬻ع鵽���б�ǵ�
			    return;
			   }
				 
				 if(u32_param[0]==USART2_BASE){
					 
					  usart_param_check[0] = pUsartRead__ID ;
					  usart_param_check[1] = USART2_Check;	
					 
	         (*(u32(*)())usmart_nametab[8].func)(usart_param_check,2);  //USART2
					 					 
					 (*(u32(*)())usmart_nametab[8].func)(toPc_regdata_U2,data_count_U2);
						 
					 

				}else if(u32_param[0]==UART4_BASE){
					
					  usart_param_check[0] = pUsartRead__ID;
					  usart_param_check[1] = UART4_Check;	
					
			     (*(u32(*)())usmart_nametab[8].func)(usart_param_check,2);    //UART4
					
					 (*(u32(*)())usmart_nametab[8].func)(toPc_regdata_U4,data_count_U4);	
											 
					 				 
		    }else{
				return;
				}				

				break;
			
			case 0xCA:
			   
			   LoopList_GetOneData(&DataBuffer,&Tmp);    //BaudRate
			   u32_param[1] = Tmp;
			   paramsCheck += Tmp;  paramsCount ++;
			
         LoopList_GetOneData(&DataBuffer,&Tmp);    //WordLength
			   u32_param[2] = Tmp<<8;
			   paramsCheck += Tmp;  paramsCount ++;
			
			   LoopList_GetOneData(&DataBuffer,&Tmp);    //StopBits
			   u32_param[3] = Tmp<<8;
			   paramsCheck += Tmp;  paramsCount ++;
			
			   LoopList_GetOneData(&DataBuffer,&Tmp);    //Parity
			   u32_param[4] = Tmp<<8;
			   paramsCheck += Tmp;  paramsCount ++;
			
			   LoopList_GetOneData(&DataBuffer,&Tmp);    //HardwareFlowControl
			   u32_param[5] = Tmp<<8;
			   paramsCheck += Tmp;  paramsCount ++;
			
			   LoopList_GetOneData(&DataBuffer,&Tmp);    //Mode
			   u32_param[6] = Tmp;
			   paramsCheck += Tmp;  paramsCount ++;
				 
			   LoopList_GetOneData(&DataBuffer,&Tmp);    //MsgMode
			   u32_param[7] = Tmp;
				 paramsCheck += Tmp;  paramsCount ++;
				 
				 LoopList_GetOneData(&DataBuffer,&Tmp);			            //������в���ֵУ��
			   if(!ParamsCheckOut(paramsCheck,paramsCount,Tmp)) return;
				 
				 LoopList_GetOneData(&DataBuffer,&Tmp);
			   if(Tmp!= pUsartInit__ID ){
					 LoopList_FeedBack(&DataBuffer,io_feature.ReadPoint,io_feature.DataSize);  //������󣬻ع鵽���б�ǵ�
			    return;
			   }
			   
         (*(u32(*)())usmart_nametab[5].func)(u32_param[0],u32_param[1],u32_param[2],
				  u32_param[3],u32_param[4],u32_param[5],u32_param[6],u32_param[7]);
				 
				break;
				 
			case 0xAA:		
				
				 LoopList_GetOneData(&DataBuffer,&Tmp);			            //������в���ֵУ��
			   if(!ParamsCheckOut(paramsCheck,paramsCount,Tmp)) return; 
			
				 LoopList_GetOneData(&DataBuffer,&Tmp);
			   if(Tmp!= pUsartDeInit__ID ){
					LoopList_FeedBack(&DataBuffer,io_feature.ReadPoint,io_feature.DataSize);  //������󣬻ع鵽���б�ǵ�
			    return;
			   }
				 
				 (*(u32(*)())usmart_nametab[6].func)(u32_param[0]);
				 
				break;
				 
			  default:	
				  return;
		}
		
		msg_feedback(USART_Check);
}



void cho_adc(void){

    uint8_t Tmp;
	  uint8_t dir;
	  uint32_t u32_param[MAX_PARAM];
	  uint8_t u8_param[ADC_MAX];
		uint8_t u8_param_length=0;
		
	  short i=0;
	
	  LoopList_GetOneData(&DataBuffer,&dir);

	  switch(dir){
			
			case 0xDA:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  u32_param[0] = Tmp;
			
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!=0xA1){
			    return;
			  }
				
			  (*(u32(*)())usmart_nametab[11].func)((uint8_t)u32_param[0]);
			  
	  break;
			
			case 0xCA:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			
			  u32_param[0] = Tmp<<12;
			 
			  for(i=0;i<ADC_MAX+1;i++){		
					
				LoopList_GetOneData(&DataBuffer,&Tmp);	
					
				if(i==ADC_MAX && Tmp!=0xA1){
				return;
				}							
				if(Tmp==0xA1){
				break;
				} 
			  	u8_param_length++;
					u8_param[i] = Tmp; 	
				}

			  (*(u32(*)())usmart_nametab[9].func)(u32_param[0],u8_param,u8_param_length);
				
			break;
				
		  	case 0xAA:
					
				LoopList_GetOneData(&DataBuffer,&Tmp);
				
				if(Tmp!=0xA1){
			    return;
			  }
			   (*(u32(*)())usmart_nametab[10].func)();
			
			break;
				
			default:
				
				  return;				
		 }
		
		 msg_feedback(ADC_Check_Last);
}

void cho_pwm(void){
	 
    unsigned char Tmp;
	  unsigned char dir;
	  unsigned char TmpBfr[4];
	  uint32_t u32_param[MAX_PARAM];
	  
	  unsigned char TmpBfr2[2];
	  uint16_t u16_param[3];
	
   	LoopList_GetOneData(&DataBuffer,&dir);
	  LoopList_GetOneData(&DataBuffer,&Tmp);
	
	   if(Tmp==0x34|Tmp==0x2c){
			 u32_param[0] = APB2PERIPH_BASE+(Tmp<<8);
		 }else{
		   u32_param[0] = APB1PERIPH_BASE+(Tmp<<8); 
		 }
    
	  switch(dir){
			
			case 0xDA:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);		  
			  u32_param[1] = Tmp;
			  LoopList_GetMultiData(&DataBuffer,TmpBfr2,2,LITTERLIN);
		  	u16_param[0] = *((uint16_t*)TmpBfr2);
			
				LoopList_GetOneData(&DataBuffer,&Tmp);				
				if(Tmp!=0xA1){
			    return;
			  }
				
			  (*(u32(*)())usmart_nametab[16].func)(u32_param[0],(uint8_t)u32_param[1],u16_param[0]);
			
				break;		
			case 0xDB:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);		  
			  u32_param[1] = Tmp;
			
				LoopList_GetOneData(&DataBuffer,&Tmp);
				if(Tmp!=0xA1){
			    return;
			  }
				
			   (*(u32(*)())usmart_nametab[17].func)(u32_param[0],(uint8_t)u32_param[1]);
			
				break;
				
			case 0xDC:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);		  
			  u32_param[1] = Tmp;
			
				LoopList_GetOneData(&DataBuffer,&Tmp);
				if(Tmp!=0xA1){
			    return;
			  }
				
			   (*(u32(*)())usmart_nametab[18].func)(u32_param[0],(uint8_t)u32_param[1]);			
				break;		
				
			case 0xDD:
				  
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			  u32_param[1] = Tmp;
				LoopList_GetOneData(&DataBuffer,&Tmp);
				if(Tmp!=0xA1){
			    return;
			  }
				
			   (*(u32(*)())usmart_nametab[19].func)(u32_param[0],(uint8_t)u32_param[1] );
				break;
				
			case 0xDE:
				
        LoopList_GetMultiData(&DataBuffer,TmpBfr2,2,LITTERLIN);
		  	u16_param[0] = *((uint16_t*)TmpBfr2);
			
				LoopList_GetOneData(&DataBuffer,&Tmp);
				if(Tmp!=0xA1){
			    return;
			   }
			  (*(u32(*)())usmart_nametab[38].func)(u32_param[0],u16_param[0] );
				break;
				 
      case 0xCA:
			
				LoopList_GetMultiData(&DataBuffer,TmpBfr,4,BIGLIN); 
	     	LoopList_GetMultiData(&DataBuffer,TmpBfr2,2,LITTERLIN);     
			  u16_param[0] = *((uint16_t*)TmpBfr2);  
			  LoopList_GetMultiData(&DataBuffer,TmpBfr2,2,LITTERLIN);
			  u16_param[1] = *((uint16_t*)TmpBfr2); 
			  LoopList_GetMultiData(&DataBuffer,TmpBfr2,2,LITTERLIN);    //���ﻹ�ǵ���
			  u16_param[2] = *((uint16_t*)TmpBfr2);
			
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!=0xA1){
				return;
				}
				
       (*(u32(*)())usmart_nametab[12].func)(u32_param[0],TmpBfr,u16_param[0],u16_param[1],u16_param[2]);			
			  
				break;
				
			case 0xCB:
				
				LoopList_GetMultiData(&DataBuffer,TmpBfr2,2,LITTERLIN); 
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			
			  if(Tmp!=0xA1){
				return;
				}
				(*(u32(*)())usmart_nametab[13].func)(u32_param[0],TmpBfr2);
			
				break;
			
		  case 0xCC:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  u32_param[1] = Tmp;
				LoopList_GetMultiData(&DataBuffer,TmpBfr,2,LITTERLIN);
			  u32_param[2] = *((uint16_t*)TmpBfr);
			
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!=0xA1){
				return;
				}
				
			 (*(u32(*)())usmart_nametab[14].func)(u32_param[0],u32_param[1], u32_param[2]);
				break;
				
			case 0xAA:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!=0xA1){
				return;
				}
				(*(u32(*)())usmart_nametab[15].func)(u32_param[0]);
			
				break;
			  default:
				
				  return;
	 	}
		
	   msg_feedback(PWM_Check_Last);
}

void cho_iic(void){

   unsigned char Tmp;
	 unsigned char Tmp1;
	 unsigned char dir;
	 unsigned char i;
	 unsigned char data_flag =  0x00;
	 uint32_t u32_param[MAX_PARAM];
	 unsigned char iic_Code[IIC_CodeNum_Max];
	 LoopList_GetOneData(&DataBuffer,&dir);
	
	 switch(dir){
		 
		 case 0xCA:
			 
		 LoopList_GetOneData(&DataBuffer,&Tmp);    //SCL
		 u32_param[0] = APB2PERIPH_BASE|(Tmp<<8);
		 LoopList_GetOneData(&DataBuffer,&Tmp);
		 Tmp1 = Tmp>>4;
		 u32_param[1]= (Tmp&0x0F)<<Tmp1;
		 
		 LoopList_GetOneData(&DataBuffer,&Tmp);    //SDA
		 u32_param[2] = APB2PERIPH_BASE|(Tmp<<8);
		 LoopList_GetOneData(&DataBuffer,&Tmp);
		 Tmp1 = Tmp>>4;
		 u32_param[3]= (Tmp&0x0F)<<Tmp1;
		 
	  	LoopList_GetOneData(&DataBuffer,&Tmp);
		  if(Tmp!=0xA1){
				return;
				}
		  
			(*(u32(*)())usmart_nametab[21].func)( u32_param[0],(uint16_t)u32_param[1],u32_param[2],u32_param[3]);	
				
			break;
				
		 case 0xDA:
			 
			 LoopList_GetOneData(&DataBuffer,&Tmp);    //�������iic���ѡ��
		   iic_cho = Tmp;
		 
		   if(iic_cho == IIC_ONE){
				 
			 SCL_cho =0;
			 SDA_cho =1;	
				 
			 }else if(iic_cho == IIC_TWO){
				 
			 SCL_cho =2;
			 SDA_cho =3;
				 
			 }else{
			 return;
			 }
			 
		   LoopList_GetOneData(&DataBuffer,&Tmp);
			 
		   if(Tmp == IIC_START){
				 
				  iic_Code[0] = Tmp;
				 
			    for(Tmp1=1;;Tmp1++){
						
					 LoopList_GetOneData(&DataBuffer,&Tmp);
						
					 iic_Code[Tmp1] = Tmp;				
					 if(data_flag == 0x00 && Tmp == IIC_STOP){
						  
					    break;		 
					 }
					 if(data_flag ==0x01)data_flag = 0x00;											
					 if(Tmp == 0x06|(Tmp>>4) == 0x06) data_flag = 0x01;
					 
					}	 
			 }else{
				 
			 return;
				 
			 }
			 
      LoopList_GetOneData(&DataBuffer,&Tmp);
		  if(Tmp!=0xA1){
				return;
				}
			
		  for(i=0;i<=Tmp1;i++){
				
				if(iic_Code[i] == 0x06 | (iic_Code[i]>>4) == 0x06){
					
					(*(u32(*)())iic_name[iic_Code[i]].func)(iic_Code[i+1]);	
					 i++;		
					
				}else{		
        	(*(u32(*)())iic_name[iic_Code[i]].func)();	
				}				
			}
			
			if(iic_waitask_Succeed==0x00){   /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
				
			    iic_waitask_Succeed =0x01;
			  	(*(u32(*)())iic_name[iic_Code[1]].func)();
				
			}
      Queue_readAllDataes();      //���ｫ���ݴ��䵽pc			
			 break;		
		} 
    msg_feedback(IIC_Check_Last); 
}


void cho_wifi(void){
	
    unsigned char Tmp;
    unsigned char dir;
	  unsigned char State_check[2];
	  uint32_t u32_param[MAX_PARAM];
	  u8 *Tmp_1;
    u8 *Tmp_2;
	
		LoopList_GetOneData(&DataBuffer,&dir);

	  switch(dir){
		
			case 0xDA:
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  u32_param[0] = Tmp;
			
			  if(Tmp >0x02){    //����У��
				return;
				}
				
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!=0xA1){
				return;
				}
				
				(*(u32(*)())usmart_nametab[25].func)( (u8)u32_param[0]);
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_STATE;
				pWifiRead_(State_check,2);
				
				break;
				
			case 0xDB:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!= 0xFF) return ;   //�������У��
			  
			  while(1){
			   LoopList_GetOneData(&DataBuffer,&Tmp);
					
				 if(Tmp == 0xFF){
           break;					 
				 }else{
				  *Tmp_1++ = Tmp;
				 }
		  	}
				
				while(1){
			   LoopList_GetOneData(&DataBuffer,&Tmp);
					
				 if(Tmp == 0xFF){
           break;					 
				 }else{
				  *Tmp_2++ = Tmp;
				 }
		  	}
				
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!=0xA1){
				return;
				}
				
				(*(u32(*)())usmart_nametab[26].func)( (char)Tmp_1,(char)Tmp_2);
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_STATE;
				pWifiRead_(State_check,2);
				
				break;
				
			case 0xDC:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!= 0xFF) return ;   //�������У��
			
			  while(1){
			   LoopList_GetOneData(&DataBuffer,&Tmp);
					
				 if(Tmp == 0xFF){
           break;					 
				 }else{
				  *Tmp_1++ = Tmp;
				 }
		  	}
				
				while(1){
			   LoopList_GetOneData(&DataBuffer,&Tmp);
					
				 if(Tmp == 0xFF){
           break;					 
				 }else{
				  *Tmp_2++ = Tmp;
				 }
		  	}
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
				u32_param[0] = Tmp;
				if(Tmp>0x04)return;
				
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!=0xA1){
				return;
				}
				(*(u32(*)())usmart_nametab[27].func)( (char)Tmp_1,(char)Tmp_2);	
				
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_STATE;
				pWifiRead_(State_check,2);				
				break;
				
		  case 0xDE:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  u32_param[0] = Tmp;
			  if(Tmp>0x01)return;
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!= 0xFF) return ;   //�������У��
			
			  while(1){
			   LoopList_GetOneData(&DataBuffer,&Tmp);
					
				 if(Tmp == 0xFF){
           break;					 
				 }else{
				  *Tmp_1++ = Tmp;
				 }
		  	}
				
			  while(1){
			   LoopList_GetOneData(&DataBuffer,&Tmp);
					
				 if(Tmp == 0xFF){
           break;					 
				 }else{
				  *Tmp_2++ = Tmp;
				 }
		  	}
				
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!=0xA1){
				return;
				}
				
			(*(u32(*)())usmart_nametab[30].func)( (u8)u32_param[0],(char)Tmp_1,(char)Tmp_2);
				
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_STATE;
				pWifiRead_(State_check,2);	
				
				break;
				
		  case 0xDF:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!= 0x5D) return ;   //�������У��
			
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!=0xA1){
				return;
				}
				
				(*(u32(*)())usmart_nametab[31].func)();
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_LinkStatus;
				pWifiRead_(State_check,2);
				ESP8266_LinkStatus =0x00;
				
				break;
				
			case 0xD9:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!= 0x9D) return ;   //�������У��
			
			  LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				}
				
				(*(u32(*)())usmart_nametab[32].func)();
				
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_ucIdLinkStatus;				
				pWifiRead_(State_check,2);
				ESP8266_ucIdLinkStatus = 0x00;
				
        break;
				
			case 0xD8:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!= 0x8A) return ;   //�������У��
			  
        LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				}
				(*(u32(*)())usmart_nametab[33].func)();
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_STATE;				
				pWifiRead_(State_check,2);				
				
				break;
				
			case 0xD7:
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!= 0x7C) return ;   //�������У��
			
			  LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				}
				
				(*(u32(*)())usmart_nametab[34].func)();
				
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_STATE;				
				pWifiRead_(State_check,2);
			
				break;
				
			case 0xD6:
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!= 0x6B) return ;   //�������У��
			
			  LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				} 
			
				(*(u32(*)())usmart_nametab[35].func)();
			
				break;
				
			case 0xD5:   //���ݷ���WIFI��ʽ���ú���  ����
				
				LoopList_GetOneData(&DataBuffer,&Tmp);
			  u32_param[0] = Tmp;
			  if(Tmp>0x01)return;
				
			  LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp > 0x05) return ;   
				u32_param[1] = Tmp;
				
				LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				} 
			
			(*(u32(*)())usmart_nametab[36].func)((u8)u32_param[0],(u8)u32_param[1]);
				return;    ////���ﲻ�ڵ������з���ֵ

        break;
				
			case 0xCA:
				
			  LoopList_GetOneData(&DataBuffer,&Tmp);
        if(Tmp!= 0x68) return ;   //�������У��
			
				LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				}
				
			(*(u32(*)())usmart_nametab[22].func)();
			
				break;	
				
			case 0xCB:
				
			  LoopList_GetOneData(&DataBuffer,&Tmp);
        if(Tmp!= 0x69) return ;   //�������У��
			
				LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				}
				
			(*(u32(*)())usmart_nametab[23].func)();
			
				break;	
				
	 	case 0xCC:
				
			  LoopList_GetOneData(&DataBuffer,&Tmp);
        if(Tmp!= 0x6A) return ;   //�������У��
			
				LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				}
				
			(*(u32(*)())usmart_nametab[24].func)();
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_STATE;				
				pWifiRead_(State_check,2);
				
				break;
			 case 0xCD:
				
			  LoopList_GetOneData(&DataBuffer,&Tmp);
        if(Tmp>0x02) return ;   
			  u32_param[0] = Tmp;
			 
				LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				}
				
		 	(*(u32(*)())usmart_nametab[28].func)((u8)u32_param[0]);
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_STATE;				
				pWifiRead_(State_check,2);
				
				break;	
				
		case 0xCE:
			  LoopList_GetOneData(&DataBuffer,&Tmp);
		    if(Tmp >0x01)return;
		    u32_param[0] = Tmp;
		
		     LoopList_GetOneData(&DataBuffer,&Tmp);
			  if(Tmp!= 0xFF) return ;   //�������У��
		    
		     while(1){
			   LoopList_GetOneData(&DataBuffer,&Tmp);
					
				 if(Tmp == 0xFF){
           break;					 
				 }else{
				  *Tmp_1++ = Tmp;
				 }
		  	}
				 
				 while(1){
			   LoopList_GetOneData(&DataBuffer,&Tmp);
					
				 if(Tmp == 0xFF){
           break;					 
				 }else{
				  *Tmp_2++ = Tmp;
				 }
		  	}
				 
				LoopList_GetOneData(&DataBuffer,&Tmp);
		    if(Tmp >0x05)return;
		    u32_param[1] = Tmp;
				
				LoopList_GetOneData(&DataBuffer,&Tmp);			
			  if(Tmp!=0xA1){
				return;
				}
			(*(u32(*)())usmart_nametab[29].func)((u8)u32_param[0],(char)Tmp_1,(char)Tmp_2,(u8)u32_param[1]);
				
				State_check[0] = Wifi_Check_One;
				State_check[1] = ESP8266_STATE;				
				pWifiRead_(State_check,2);
				
        break;			
		}			
				
		 msg_feedback(Wifi_Check_Last);
     ESP8266_STATE = ESP8266_RIGHT;
		
}