#include "stdafx.h"
#include "MPWM.h"

#define PWM_pPWMInit            0xCA
#define PWM_pPWMICInit          0xCB
#define PWM_pPluseInit          0xCC
#define PWM_pPWMDeInit          0xAA
#define PWM_pPWMSetPluseWid     0xDA
#define PWM_pPWMSetFrequency    0xDE
#define PWM_CheckLast           0xA1


std::pair<unsigned short, unsigned char*> MPWM::PWMInit(PWMTIM_Type TIM, PWMOCCHANNEL_Type *Channel,
	unsigned short Period, unsigned short Prescalar, unsigned short Pluse) {

	unsigned char *sendout_;
	unsigned short length_ = 14;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

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

	sendout_[13] = PWM_CheckLast;

	return { length_ ,sendout_ };

}

std::pair<unsigned short, unsigned char*> MPWM::PWMICInit(PWMTIM_Type TIM, PWMICCHANNEL_Type *Channel) {

	unsigned char *sendout_;
	unsigned short length_ = 6;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

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

	sendout_[5] = PWM_CheckLast;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMPluseInit(PWMTIM_Type TIM, PWMENCODER_Type Mode, unsigned short Circle) {

	unsigned char *sendout_;
	unsigned short length_ = 7;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

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

	sendout_[6] = PWM_CheckLast;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMDeInit(PWMTIM_Type TIM) {

	unsigned char *sendout_;
	unsigned short length_ = 4;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));
	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMDeInit;
	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = PWM_CheckLast;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMSetPluseWid(PWMTIM_Type TIM, PWMOCCHANNEL_Type Channel, unsigned short PluseWid) {

	unsigned char *sendout_;
	unsigned short length_ = 7;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

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

	sendout_[6]= PWM_CheckLast;

	return { length_ ,sendout_ };
}


std::pair<unsigned short, unsigned char*> MPWM::PWMICGetPluseWid(PWMTIM_Type TIM, PWMICVALUE_Type Channel) {

	unsigned char *sendout_;
	unsigned short length_ = 5;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMICGetPluseWid;

	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Channel;
	if (!IS_PARAMS(IS_TIM_ICGETVALUE(Channel))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}

	sendout_[4] = PWM_CheckLast;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMICGetFrequency(PWMTIM_Type TIM, PWMICCHANNEL_Type Channel) {

	unsigned char *sendout_;
	unsigned short length_ = 5;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMICGetFrequancy;
	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Channel;
	if (!IS_PARAMS(IS_TIM_ICCHANNEL(Channel))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}

	sendout_[4] = PWM_CheckLast;

	return { length_ ,sendout_ };

}

std::pair<unsigned short, unsigned char*> MPWM::PWMGetPluse(PWMTIM_Type TIM, PWM_ENCODERCHANNEL_Type Channel) {

	unsigned char *sendout_;
	unsigned short length_ = 5;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

	PWM_RIGHT = PWMRIGHT;

	sendout_[0] = PWM_Check;
	sendout_[1] = PWM_pPWMGetPluse;

	sendout_[2] = TIM;
	if (!IS_PARAMS(IS_TIM_LIST8_PERIPH(TIM))) { PWM_RIGHT = 0x01; return { length_ ,sendout_ };
	}
	sendout_[3] = Channel;
	if (!IS_PARAMS(IS_TIM_ENCODERCHANNEL(Channel))) { PWM_RIGHT = 0x02; return { length_ ,sendout_ };
	}

	sendout_[4] = PWM_CheckLast;

	return { length_ ,sendout_ };
}

std::pair<unsigned short, unsigned char*> MPWM::PWMSetFrequency(PWMTIM_Type TIM, unsigned short Prescaler) {

	unsigned char *sendout_;
	unsigned short length_ = 6;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));
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

	sendout_[5] = PWM_CheckLast;

	return { length_ ,sendout_ };

}



unsigned char* MPWM::MPWMSerialFeedBack() {

	unsigned char *sendout_;
	unsigned short length_ = 3;
	sendout_ = (unsigned char*)malloc(length_ * sizeof(unsigned char));

	*sendout_-- = 0xA1;
	*sendout_-- = 0xDC;
	*sendout_ = 0xFA;

	return sendout_;
}


unsigned char* MPWM::MPWMWifiFeedBack(unsigned char ID) {

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



bool MPWM::IS_PARAMS(bool JUDGE) {
	if (JUDGE) return true; else return false;
}

void MPWM::s_To_h(const unsigned short *val, unsigned char hex[2]) {

	const char *p = (char*)(val);
	hex[0] = *((char*)p);
	hex[1] = *((char*)(p + 1));

}






