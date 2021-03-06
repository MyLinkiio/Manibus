#include "stdafx.h"
#include "MGpio.h"


#define GPIO_pPinInit                   0xCA
#define GPIO_pPinDeInit                 0xAA
#define GPIO_pPinOutPut                 0xDA
#define GPIO_pPinReadBit                0xDB

#define GPIOBUFFERMAXSIZE               50

MGPIO::MGPIO() {

	sendout_ = (unsigned char*)malloc(GPIOBUFFERMAXSIZE * sizeof(unsigned char));
}
MGPIO::~MGPIO() {
	
	free(sendout_);
}

std::pair<unsigned short, unsigned char*> MGPIO::PinInit(GPIOIO_Type IO, GPIOPIN_Type Pin, GPIOMODE_Type Mode, GPIOSpeed_Type Speed) {

	GPIO_FBInit();
	unsigned short length_ = 8;

	GPIO_RIGHT = GPIORIGHT;

	sendout_[0] = GPIO_Check;
	sendout_[1] = GPIO_pPinInit;

	sendout_[2] = IO;
	if (!IS_PARAMS(IS_GPIO_ALL_PERIPH(IO))) { GPIO_RIGHT = 0x01;  return { length_ ,sendout_ }; 
	}

	sendout_[3] = Pin;
	if (!IS_PARAMS(IS_GET_GPIO_PIN(Pin))) { GPIO_RIGHT = 0x02;  return { length_ ,sendout_ };
	}

	sendout_[4] = Mode;
	if (!IS_PARAMS(IS_GPIO_MODE(Mode))) { GPIO_RIGHT = 0x03; return { length_ ,sendout_ };
	}

	sendout_[5] = Speed;
	if (!IS_PARAMS(IS_GPIO_SPEED(Speed))) { GPIO_RIGHT = 0x04;  return { length_ ,sendout_ };
	}

	short temp = 0;

	for (char i = 2; i < 6; i++) {
		
		temp += sendout_[i];
	}
	sendout_[6] = temp % 0xFF;


	sendout_[7] = pPinInit__ID;

	return { length_ ,sendout_ };

}


std::pair<unsigned short, unsigned char*> MGPIO::PinDeInit(GPIOIO_Type IO) {

	GPIO_FBInit();
	unsigned short length_ = 5;	

	GPIO_RIGHT = GPIORIGHT;

	sendout_[0] = GPIO_Check;
	sendout_[1] = GPIO_pPinDeInit;

	sendout_[2] = IO;
	if (!IS_PARAMS(IS_GPIO_ALL_PERIPH(IO))) { GPIO_RIGHT = 0x01; return { length_ ,sendout_ };
	}

	sendout_[3] = sendout_[2] % 0xFF;
	sendout_[4] = pPinDeInit__ID;

	return { length_,sendout_ };
}

std::pair<unsigned short, unsigned char*> MGPIO::PinOutPut(GPIOIO_Type IO, GPIOPIN_Type Pin, BitAction Level) {

	GPIO_FBInit();
	unsigned short length_ = 7;

	 GPIO_RIGHT = GPIORIGHT;
	 sendout_[0] = GPIO_Check;
	 sendout_[1] = GPIO_pPinOutPut;

	 sendout_[2] = IO;
	 if (!IS_PARAMS(IS_GPIO_ALL_PERIPH(IO))) { GPIO_RIGHT = 0x01; return { length_,sendout_ };
	 }

	 sendout_[3] = Pin;
	 if (!IS_PARAMS(IS_GET_GPIO_PIN(Pin))) { GPIO_RIGHT = 0x02; return { length_,sendout_ };
	 }

	 sendout_[4] = Level;
	 if (!IS_PARAMS(IS_GPIO_BIT_ACTION(Level))) { GPIO_RIGHT = 0x03;  return { length_,sendout_ };
	 }

	 short temp = 0;

	 for (char i = 2; i < 5; i++) {

		 temp += sendout_[i];
	 }

	 sendout_[5] = temp % 0xFF;
	 sendout_[6] = pPinOutPut__ID;

	 return { length_,sendout_ };
}

std::pair<unsigned short, unsigned char*> MGPIO::PinReadBit(GPIOIO_Type IO, GPIOPIN_Type Pin) {

	GPIO_FBInit();
	unsigned short length_ = 6;

	GPIO_RIGHT = GPIORIGHT;

	sendout_[0] = GPIO_Check;
	sendout_[1] = GPIO_pPinReadBit;

	sendout_[2] = IO;
	if (!IS_PARAMS(IS_GPIO_ALL_PERIPH(IO))) { GPIO_RIGHT = 0x01; return { length_,sendout_ };
	}

	sendout_[3] = Pin;
	if (!IS_PARAMS(IS_GET_GPIO_PIN(Pin))) { GPIO_RIGHT = 0x02;  return { length_,sendout_ };
	}

	sendout_[4] = (sendout_[2] + sendout_[3]) % 0xFF;
	sendout_[5] = pPinReadBit__ID;

	return { length_,sendout_ };
 }

std::pair<unsigned short, unsigned char*>  MGPIO::MGPIOSerialFeedBack() {

	unsigned short length_ = 3;

	sendout_[0] = 0xFA;
	sendout_[1] = 0xDC;
	sendout_[2] = 0xA5;

	return { length_,sendout_ };
}

std::pair<unsigned short, unsigned char*>  MGPIO::MGPIOWifiFeedBack(unsigned char ID) {

	
	unsigned short length_ = 5;	

	sendout_[0] = 0xF9;
	sendout_[1] = 0xD5;
	sendout_[2] = 0x01;
	sendout_[3] = ID;
	sendout_[4] = 0x9F;

	return{ length_,sendout_ };
}

bool MGPIO::GPIO_FeedBack(unsigned char ID, unsigned char *Msg) {

	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID)) {
			GPIO_ID_CHECK_ = true;
			continue_count++;
		}
		else if (IS_RIGHT(Msg[i], GPIO_Check)) {
			GPIO_CHECK_ = true;
			continue_count++;
		}

	}

	if (GPIO_ID_CHECK_ && GPIO_CHECK_ && continue_count == 2) {
		return true;
	}
	return false;

}

bool MGPIO::GPIO_FeedBack(unsigned char ID, unsigned char *Msg, unsigned char * readback) {

	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	short length_ = 0;
	if (ID == pPinReadBit__ID) length_ = 1;
	else
		return false;


	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID) && continue_count == 0) {

			GPIO_ID_CHECK_ = true;
			continue_count++;
		}
		else if (continue_count > 0 && continue_count < length_+1) {

			sendout_[continue_count-1] = Msg[i];

			continue_count++;
		}
		else if (continue_count == length_ + 1) {

			if (IS_RIGHT(Msg[i], GPIO_Check)) {
				GPIO_CHECK_ = true;
				continue_count++;
			}
		}
	}

	if (GPIO_ID_CHECK_ && GPIO_CHECK_ && continue_count == length_ +2) {
		memcpy(readback, sendout_, 1);
		return true;
	}

	return false;
}


bool MGPIO::IS_PARAMS(bool JUDGE) {
	if (JUDGE) return true; else return false;
}

bool MGPIO::IS_RIGHT(unsigned char param1, unsigned char param2) {

	if (param1 == param2) {
		return true;
	}
	else {
		return false;
	}

}
