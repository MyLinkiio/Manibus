#include "stdafx.h"
#include "MADC.h"

#define ADC_pADCInit     0xCA
#define ADC_pADCDeInit   0xAA
#define ADC_pADCRead     0xDA
#define ADC_CheckLast    0xA1

//取数组地址为错误  //除法 整数型转化 //memcpy地址重新定向开头  此时不可以使用地址操作//malloc 申请地址后指向最后
std::pair<unsigned short, unsigned char*> MADC::ADC_Init(ADCSPEED_Type Speed, ADCCHANNEL_Type *Channel) {

	ADC_RIGHT = ADCRIGHT;

	unsigned char *sendout_;
	unsigned short param_length = (unsigned short)(sizeof(Channel) / sizeof(Channel[0]));//******
	unsigned short length_ = 4 + param_length;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

	sendout_[0] = ADC_Check;
	sendout_[1] = ADC_pADCInit;
	sendout_[2] = Speed;

	if (!IS_PARAMS(IS_ADC_SPEED(Speed))) { ADC_RIGHT = 0x01; return { length_, sendout_ }; }

	memcpy(sendout_+3, Channel, param_length);

	for (char i = 0; i < param_length; i++) {
		if (!IS_PARAMS(IS_ADC_CHANNEL(Channel[i]))) { ADC_RIGHT = 0x02; return { length_, sendout_ }; }
	}

	sendout_[3 + param_length] = ADC_CheckLast;

	return { length_, sendout_ };

}

std::pair<unsigned short, unsigned char*> MADC::ADC_DeInit() {

	ADC_RIGHT = ADCRIGHT;

	unsigned char *sendout_;
	unsigned short length_ = 3;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

	sendout_[0] = ADC_Check;
	sendout_[1] = ADC_pADCDeInit;
	sendout_[2] = ADC_CheckLast;

	return { length_, sendout_ };
}

std::pair<unsigned short, unsigned char*> MADC::ADC_Read(ADCCHANNEL_Type Channel) {

	ADC_RIGHT = ADCRIGHT;

	unsigned char *sendout_;
	unsigned short length_ = 4;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

	sendout_[0] = ADC_Check;
	sendout_[1] = ADC_pADCRead;
	sendout_[2] = Channel;

	if (!IS_PARAMS(IS_ADC_CHANNEL(Channel))) { ADC_RIGHT = 0x01; return { length_, sendout_ }; }

	sendout_[3] = ADC_CheckLast;

	return { length_, sendout_ };

}

unsigned char* MADC::MADCSerialFeedBack() {

	unsigned char *sendout_;
	unsigned short length_ = 3;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

	*sendout_-- = 0xA1;
	*sendout_-- = 0xDC;
	*sendout_ = 0xFA;

	return sendout_;

}

unsigned char* MADC::MADCWifiFeedBack(unsigned char ID) {

	unsigned char *sendout_;
	unsigned short length_ = 5;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

	*sendout_-- = 0xA1;
	*sendout_-- = ID;
	*sendout_-- = 0x01;
	*sendout_-- = 0xD5;
	*sendout_ = 0xF9;

	return sendout_;

}
bool MADC::IS_PARAMS(bool JUDGE) {
	if (JUDGE) return true; else return false;
}










