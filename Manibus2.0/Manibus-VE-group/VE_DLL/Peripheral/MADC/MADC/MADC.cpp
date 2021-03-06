#include "stdafx.h"
#include "MADC.h"

#define ADC_pADCInit     0xCA
#define ADC_pADCDeInit   0xAA
#define ADC_pADCRead     0xDA

#define ADCBUFFERMAXSIZE 50

MADC::MADC() {

	sendout_ = (unsigned char*)malloc(ADCBUFFERMAXSIZE * sizeof(unsigned char));

}

MADC::~MADC() {

	free(sendout_);
}

//取数组地址为错误  //除法 整数型转化 //memcpy地址重新定向开头  此时不可以使用地址操作//malloc 申请地址后指向最后
std::pair<unsigned short, unsigned char*> MADC::ADCInit(ADCSPEED_Type Speed, ADCCHANNEL_Type *Channel) {

	ADC_FBInit();

	ADC_RIGHT = ADCRIGHT;

	unsigned short param_length = (unsigned short)(sizeof(Channel) / sizeof(Channel[0]));//******
	
	unsigned short length_ = 6 + param_length;
	

	sendout_[0] = ADC_Check;
	sendout_[1] = ADC_pADCInit;
	sendout_[2] = param_length;

	sendout_[3] = Speed;
	if (!IS_PARAMS(IS_ADC_SPEED(Speed))) { ADC_RIGHT = 0x01; return { length_, sendout_ }; }


	memcpy(sendout_+4, Channel, param_length);

	unsigned short temp = 0;

	temp += (sendout_[2]+ sendout_[3]);

	for (char i = 0; i < param_length; i++) {
		if (!IS_PARAMS(IS_ADC_CHANNEL(Channel[i]))) { ADC_RIGHT = 0x02; return { length_, sendout_ }; }
		temp += Channel[i];
	}

	sendout_[4 + param_length] = temp % 0xFF;

	sendout_[5 + param_length] = pADCInit__ID;

	return { length_, sendout_ };

}

std::pair<unsigned short, unsigned char*> MADC::ADCDeInit() {

	ADC_FBInit();

	ADC_RIGHT = ADCRIGHT;

	unsigned short length_ = 3;

	sendout_[0] = ADC_Check;
	sendout_[1] = ADC_pADCDeInit;
	sendout_[2] = pADCDeInit__ID;

	return { length_, sendout_ };
}

std::pair<unsigned short, unsigned char*> MADC::ADCRead(ADCCHANNEL_Type Channel) {

	ADC_FBInit();

	ADC_RIGHT = ADCRIGHT;

	unsigned short length_ = 5;

	sendout_[0] = ADC_Check;
	sendout_[1] = ADC_pADCRead;
	sendout_[2] = Channel;
	sendout_[3] = sendout_[2] % 0xFF;

	if (!IS_PARAMS(IS_ADC_CHANNEL(Channel))) { ADC_RIGHT = 0x01; return { length_, sendout_ }; }

	sendout_[4] = pADCRead__ID;

	return { length_, sendout_ };

}

std::pair<unsigned short, unsigned char*> MADC::MADCSerialFeedBack() {

	unsigned short length_ = 3;

	sendout_[0] = 0xFA;
	sendout_[1] = 0xDC;
	sendout_[2] = 0xA5;

	return { length_, sendout_ };

}


std::pair<unsigned short, unsigned char*> MADC::MADCWifiFeedBack(unsigned char ID) {

	unsigned short length_ = 5;

	sendout_[0] = 0xF9;
	sendout_[1] = 0xD5;
	sendout_[2] = 0x01;
	sendout_[3] = ID;
	sendout_[4] = 0x9F;

	return { length_, sendout_ };

}


bool MADC::ADC_FeedBack(unsigned char ID, unsigned char *Msg) {

	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID)) {
			ADC_ID_CHECK_ = true;
			continue_count++;
		}
		else if (IS_RIGHT(Msg[i], ADC_Check)) {
			ADC_CHECK_ = true;
			continue_count++;
		}
	}

	if (ADC_ID_CHECK_ && ADC_CHECK_ && continue_count == 2) {
		return true;
	}
	return false;

}

bool MADC::ADC_FeedBack(unsigned char ID, unsigned char *Msg, unsigned char* readback) {

	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	short length_ = 0;
	if (ID == pADCRead__ID)length_ = 4;
	else
		return false;

	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID) && !ADC_ID_CHECK_) {

			ADC_ID_CHECK_ = true;
			continue_count++;
		}
		else if (continue_count>0 && continue_count<length_+1) {

			sendout_[continue_count - 1] = Msg[i];
			continue_count++;
		}
		else if (continue_count == length_ +1) {

			unsigned short temp = 0;

			for (char j = 0; j < length_; j++) {

				temp += sendout_[j];
			}

			if (Msg[i] == temp % 0xFF) {
				ADC_PARAM_CHECK_ = true;
				continue_count++;
			}			

		}
		else if (continue_count == length_+2) {
			if (IS_RIGHT(Msg[i], ADC_Check)) ADC_CHECK_ = true;
		}
	}

	if (ADC_CHECK_ && ADC_PARAM_CHECK_ && ADC_ID_CHECK_) {
		memcpy(readback, sendout_, 4);
		return true;
	}
	return false;

}

bool MADC::IS_PARAMS(bool JUDGE) {
	if (JUDGE) return true; else return false;
}

bool MADC::IS_RIGHT(unsigned char param1, unsigned char param2) {

	if (param1 == param2) {
		return true;
	}
	else {
		return false;
	}

}








