#include "stdafx.h"
#include "MPWM.h"

#define PWM_pPWMInit            0xCA
#define PWM_pPWMICInit          0xCB
#define PWM_pPluseInit          0xCC
#define PWM_pPWMDeInit          0xAA
#define PWM_pPWMSetPluseWid     0xDA
#define PWM_pPWMSetFrequency    0xDE


MPWM::MPWM() {

	sendout_ = (unsigned char*)malloc(50 * sizeof(unsigned char));
}


MPWM::~MPWM() {

	free(sendout_);
}


std::pair<unsigned short, unsigned char*> MPWM::PWMInit(PWMTIM_Type TIM, PWMOCCHANNEL_Type *Channel,
	unsigned short Period, unsigned short Prescalar, unsigned short Pluse) {

	PWM_FBInit();
	unsigned short length_ = 15;	

	PWM_RIGHT = PWMRIGHT;
	unsigned char params2hex[2];

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMInit;
	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))){ PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Channel[0];
	if (!IS_PARAMS(IS_TIM_OCCHANNEL(Channel[0]))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}
	sendout_[4] = Channel[1];
	if (!IS_PARAMS(IS_TIM_OCCHANNEL(Channel[1]))) { PWM_RIGHT = 0x03; return { length_ ,sendout_ };
	}
	sendout_[5] = Channel[2];
	if (!IS_PARAMS(IS_TIM_OCCHANNEL(Channel[2]))) { PWM_RIGHT = 0x04; return { length_ ,sendout_ };
	}
	sendout_[6] = Channel[3];
	if (!IS_PARAMS(IS_TIM_OCCHANNEL(Channel[3]))) { PWM_RIGHT = 0x05; return { length_ ,sendout_ };
	}

	s_To_h(&Period, params2hex);
	memcpy(sendout_+7, params2hex,2);

	s_To_h(&Prescalar, params2hex);
	memcpy(sendout_ + 9, params2hex, 2);

	s_To_h(&Pluse, params2hex);
	memcpy(sendout_ + 11, params2hex, 2);

	short temp = 0;

	for (char i = 2; i < 13; i++) {

		temp += sendout_[i];
	}
	sendout_[13] = temp % 0xFF;

	sendout_[14] = pPWMInit__ID;

	return { length_ ,sendout_ };

}

std::pair<unsigned short, unsigned char*> MPWM::PWMICInit(PWMTIM_Type TIM, PWMICCHANNEL_Type *Channel) {

	PWM_FBInit();

	unsigned short length_ = 7;

	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMICInit;
	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Channel[0];
	if (!IS_PARAMS(IS_TIM_ICCHANNEL(Channel[0]))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}
	sendout_[4] = Channel[1];
	if (!IS_PARAMS(IS_TIM_ICCHANNEL(Channel[1]))) { PWM_RIGHT = 0x03; return { length_ ,sendout_ };
	}

	short temp = 0;

	for (char i = 2; i < 5; i++) {

		temp += sendout_[i];
	}
	sendout_[5] = temp % 0xFF;

	sendout_[6] = pPWMICInit__ID;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMPluseInit(PWMTIM_Type TIM, PWMENCODER_Type Mode, unsigned short Circle) {

	PWM_FBInit();

	unsigned short length_ = 8;

	PWM_RIGHT = PWMRIGHT;
	unsigned char params2hex[2];

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPluseInit;
	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Mode;
	if (!IS_PARAMS(IS_TIM_ENCODERMODE(Mode))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}

	s_To_h(&Circle, params2hex);
	memcpy(sendout_+4, params2hex,2);

	short temp = 0;

	for (char i = 2; i < 6; i++) {

		temp += sendout_[i];
	}
	sendout_[6] = temp % 0xFF;

	sendout_[7] = pPWMPluseInit__ID;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMDeInit(PWMTIM_Type TIM) {

	PWM_FBInit();

	unsigned short length_ = 5;

	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMDeInit;
	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}

	sendout_[3] = sendout_[2] % 0xFF;
	sendout_[4] = pPWMDeInit__ID;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMSetPluseWid(PWMTIM_Type TIM, PWMOCCHANNEL_Type Channel, unsigned short PluseWid) {


	PWM_FBInit();

	unsigned short length_ = 8;

	PWM_RIGHT = PWMRIGHT;
	unsigned char params2hex[2];

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMSetPluseWid;

	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Channel;
	if (!IS_PARAMS(IS_TIM_OCCHANNEL(Channel))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}

	s_To_h(&PluseWid, params2hex);
	memcpy(sendout_ + 4, params2hex, 2);

	short temp = 0;

	for (char i = 2; i < 6; i++) {

		temp += sendout_[i];
	}
	sendout_[6] = temp % 0xFF;

	sendout_[7]= pPWMSetPluseWid__ID;

	return { length_ ,sendout_ };
}


std::pair<unsigned short, unsigned char*> MPWM::PWMICGetPluseWid(PWMTIM_Type TIM, PWMICVALUE_Type Channel) {

	PWM_FBInit();

	unsigned short length_ = 6;

	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMICGetPluseWid;

	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Channel;
	if (!IS_PARAMS(IS_TIM_ICGETVALUE(Channel))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}

	sendout_[4] = (sendout_[2] + sendout_[3]) % 0xFF;
	sendout_[5] = pPWMICGetPluseWid__ID;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMICGetFrequency(PWMTIM_Type TIM, PWMICCHANNEL_Type Channel) {

	PWM_FBInit();

	unsigned short length_ = 6;

	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMICGetFrequancy;
	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Channel;
	if (!IS_PARAMS(IS_TIM_ICCHANNEL(Channel))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}

	sendout_[4] = (sendout_[2] + sendout_[3]) % 0xFF;
	sendout_[5] = pPWMICGetFrequency__ID;

	return { length_ ,sendout_ };

}

std::pair<unsigned short, unsigned char*> MPWM::PWMGetPluse(PWMTIM_Type TIM, PWM_ENCODERCHANNEL_Type Channel) {

	PWM_FBInit();

	unsigned short length_ = 6;


	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMGetPluse;

	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Channel;
	if (!IS_PARAMS(IS_TIM_ENCODERCHANNEL(Channel))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}

	sendout_[4] = (sendout_[2] + sendout_[3]) % 0xFF;
	sendout_[5] = pPWMGetPluse__ID;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMSetFrequency(PWMTIM_Type TIM, unsigned short Prescaler) {

	PWM_FBInit();

	unsigned short length_ = 7;

	unsigned char params2hex[2];

	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMSetFrequency;
	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) {
		PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}

	s_To_h(&Prescaler, params2hex);
	memcpy(sendout_ + 3, params2hex, 2);


	short temp = 0;

	for (char i = 2; i < 5; i++) {

		temp += sendout_[i];
	}
	sendout_[5] = temp % 0xFF;

	sendout_[6] = pPWMSetFrequency__ID;

	return { length_ ,sendout_ };

}


std::pair<unsigned short, unsigned char*> MPWM::MPWMSerialFeedBack() {

	unsigned short length_ = 3;

	sendout_[0] = 0xFA;
	sendout_[1] = 0xDC;
	sendout_[2] = 0xA5;

	return { length_ ,sendout_ };
}


std::pair<unsigned short, unsigned char*> MPWM::MPWMWifiFeedBack(unsigned char ID) {

	unsigned short length_ = 5;

	sendout_[0] = 0xF9;
	sendout_[1] = 0xD5;
	sendout_[2] = 0x01;
	sendout_[3] = ID;
	sendout_[4] = 0x9F;

	return { length_ ,sendout_ };
}


bool MPWM::PWM_FeedBack(unsigned char ID, unsigned char* Msg) {

	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID)) {
			PWM_ID_CHECK_ = true;
			continue_count++;
		}
		else if (IS_RIGHT(Msg[i], PWM_Check)) {
			PWM_CHECK_ = true;
			continue_count++;
		}
	}

	if (PWM_ID_CHECK_ && PWM_CHECK_ && continue_count == 2) {
		return true;
	}
	return false;
}


bool MPWM::PWM_FeedBack(unsigned char ID, unsigned char* Msg, unsigned char* readback) {

	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	short length_ = 0;

	if (ID == pPWMICGetPluseWid__ID || ID == pPWMICGetFrequency__ID || pPWMGetPluse__ID)length_ = 4;
	else
		return false;

	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID) && !PWM_ID_CHECK_) {

			PWM_ID_CHECK_ = true;
			continue_count++;
		}
		else if (continue_count>0 && continue_count<length_ +1) {

			sendout_[continue_count - 1] = Msg[i];
			continue_count++;
		}
		else if (continue_count == length_ + 1) {

			unsigned short temp = 0;

			for (char j = 0; j < length_; j++) {

				temp += sendout_[j];
			}

			if (Msg[i] == temp % 0xFF) {
				PWM_PARAM_CHECK_ = true;
			}

		}
		else if (continue_count == length_ + 2) {
			if (IS_RIGHT(Msg[i], PWM_Check)) PWM_CHECK_ = true;
		}
	}

	if (PWM_CHECK_ && PWM_PARAM_CHECK_ && PWM_ID_CHECK_) {
		memcpy(readback, sendout_, length_);
		return true;
	}
	return false;
}

bool MPWM::IS_PARAMS(bool JUDGE) {
	if (JUDGE) return true; else return false;
}

bool MPWM::IS_RIGHT(unsigned char param1, unsigned char param2) {

	if (param1 == param2) {
		return true;
	}
	else {
		return false;
	}

}

void MPWM::s_To_h(const unsigned short *val, unsigned char hex[2]) {

	const char *p = (char*)(val);
	hex[0] = *((char*)p);
	hex[1] = *((char*)(p + 1));

}






