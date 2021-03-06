#include "stdafx.h"
#include "MUsart.h"

#define USART_pUsartInit     0xCA
#define USART_pUsartDeInit   0xAA
#define USART_pUsartWrite    0xDA
#define USART_pUsartRead     0xDB
#define USART_pUsartOnLine   0xDC


MUSART::MUSART() {

	sendout_ = (unsigned char*)malloc(350 * sizeof(unsigned char));

}

MUSART::~MUSART() {

	free(sendout_);
}

std::pair<unsigned short, unsigned char*> MUSART::UsartInit(USART_Type Usart, USARTBAUD_Type Baud,
	USARTWORDLEN_Type WordLength, USARTSTOPBIT_Type StopBits, USARTPARITY_Type Parity,
	USARTHFC_Type HFC, USARTMODE_Type Mode,unsigned char MsgMode) {

	USART_FBInit();
	unsigned short length_ = 12;


	USART_RIGHT = UARTRIGHT;

	sendout_[0] = USART_Check;
	sendout_[1] = USART_pUsartInit;

	sendout_[2] = Usart;
	if (!IS_PARAMS(IS_USART_1234_PERIPH(Usart))) { USART_RIGHT = 0x01; return { length_, sendout_ }; }

	sendout_[3] = Baud;
	if (!IS_PARAMS(IS_USART_BAUD(Baud))) { USART_RIGHT = 0x02; return { length_, sendout_ }; }

	sendout_[4] = WordLength;
	if (!IS_PARAMS(IS_USART_WORD_LENGTH(WordLength))) { USART_RIGHT = 0x03; return { length_, sendout_ }; }

	sendout_[5] = StopBits;
	if (!IS_PARAMS(IS_USART_STOPBITS(StopBits))) { USART_RIGHT = 0x04; return { length_, sendout_ }; }

	sendout_[6] = Parity;
	if (!IS_PARAMS(IS_USART_PARITY(Parity))) { USART_RIGHT = 0x05; return { length_, sendout_ }; }

	sendout_[7] = HFC;
	if (!IS_PARAMS(IS_USART_HARDWARE_FLOW_CONTROL(HFC))) { USART_RIGHT = 0x06; return { length_, sendout_ }; }

	sendout_[8] = Mode;
	reg_msg_length = Mode;
	if (!IS_PARAMS(IS_USART_MODE(Mode))) { USART_RIGHT = 0x07; return { length_, sendout_ }; }

	sendout_[9] = MsgMode;

	short temp = 0;

	for (char i = 2; i < 10; i++) {

		temp += sendout_[i];
	}

	sendout_[10] = temp % 0xFF;


	sendout_[11] = pUsartInit__ID;

	return { length_,sendout_ };
}

std::pair<unsigned short, unsigned char*> MUSART::UsartDeInit(USART_Type Usart) {

	USART_FBInit();

	unsigned short length_ = 5;

	USART_RIGHT = UARTRIGHT;

	sendout_[0] = USART_Check;
	sendout_[1] = USART_pUsartDeInit;

	sendout_[2] = Usart;
	if (!IS_PARAMS(IS_USART_1234_PERIPH(Usart))) { USART_RIGHT = 0x01; return { length_, sendout_ }; }

	sendout_[3] = sendout_[2] % 0xFF;

	sendout_[4] = pUsartDeInit__ID;

	return { length_,sendout_ };
}

std::pair<unsigned short,unsigned char*> MUSART::UsartWrite(USART_Type Usart, unsigned char * Words, unsigned char Length) {

	USART_FBInit();

	unsigned short length_ = 7 + Length;


	USART_RIGHT = UARTRIGHT;    	

	sendout_[0] = USART_Check;
	sendout_[1] = USART_pUsartWrite;

	sendout_[2] = Usart;
	if (!IS_PARAMS(IS_USART_1234_PERIPH(Usart))) { USART_RIGHT = 0x01; return { length_, sendout_ }; }
	sendout_[3] = Length;

	sendout_[4] = (Usart + Length) % 0xFF;

	memcpy(sendout_+5, Words, Length);

	short temp = 0;

	for (char i = 0; i < Length; i++) {

		temp += Words[i];
	}

	sendout_[length_ - 2] = temp % 0xFF;
	
	sendout_[length_ - 1] = pUsartWrite__ID;

	return { length_,sendout_ };

}

std::pair<unsigned short, unsigned char*> MUSART::UsartRead(USART_Type Usart) {

	USART_FBInit();

	unsigned short length_ = 5;


	USART_RIGHT = UARTRIGHT;

	sendout_[0] = USART_Check;
	sendout_[1] = USART_pUsartRead;

	sendout_[2] = Usart;
	if (!IS_PARAMS(IS_USART_1234_PERIPH(Usart))) { USART_RIGHT = 0x01; return { length_, sendout_ }; }

	sendout_[3] = sendout_[2] % 0xFF;

	sendout_[4] = pUsartRead__ID;

	return { length_,sendout_ };

}

std::pair<unsigned short, unsigned char*> MUSART::UsartOnLine() {

	unsigned short length_ = 3;

	sendout_[0] = USART_Check;
	sendout_[1] = USART_pUsartOnLine;
	sendout_[2] = pUsartOnLine__ID;

	return { length_,sendout_ };
}

bool MUSART::USART_FeedBack(unsigned char ID, unsigned char *Msg) {

	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID)) {
			USART_ID_CHECK_ = true;
			continue_count++;
		}
		else if (IS_RIGHT(Msg[i], USART_Check)) {
			USART_CHECK_ = true;
			continue_count++;
		}
	}

	if (USART_ID_CHECK_ && USART_CHECK_ && continue_count == 2) {
		return true;
	}
	return false;

}

bool MUSART::USART_FeedBack(unsigned char ID, unsigned char *Msg, unsigned char* readback) {

	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID) && !USART_ID_CHECK_) {

			USART_ID_CHECK_ = true;
			continue_count++;
		}
		else if (continue_count>0 && continue_count< 2) {

			sendout_[continue_count - 1] = Msg[i];
			continue_count++;
		}
		else if (continue_count>1 && continue_count <reg_msg_length +2) {
		
			sendout_[continue_count - 1] = Msg[i];
			continue_count++;
		}
		else if (continue_count == reg_msg_length + 2) {

			unsigned short temp = 0;

			for (char j = 0; j < reg_msg_length+1; j++) {

				temp += sendout_[j];
			}

			if (Msg[i] == temp % 0xFF) {
				USART_PARAM_CHECK_ = true;
				continue_count++;
			}

		}
		else if (continue_count == reg_msg_length + 3) {
			if (IS_RIGHT(Msg[i], USART_Check))USART_CHECK_ = true;
		}
	}

	if (USART_CHECK_ && USART_PARAM_CHECK_ && USART_ID_CHECK_) {
		memcpy(readback, sendout_+1, reg_msg_length);
		return true;
	}
	return false;

}

bool MUSART::IS_PARAMS(bool JUDGE) {
	if (JUDGE) return true; else return false;
}

bool MUSART::IS_RIGHT(unsigned char param1, unsigned char param2) {

	if (param1 == param2) {
		return true;
	}
	else {
		return false;
	}


}
void MUSART::s_To_h(const unsigned short *val, char hex[2]) {

	const char *p = (char*)(val);
	hex[0] = *((char*)p);
	hex[1] = *((char*)(p + 1));

}

