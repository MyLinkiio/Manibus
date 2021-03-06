#include "stdafx.h"
#include "MUsart.h"

#define USART_pUsartInit     0xCA
#define USART_pUsartDeInit   0xAA
#define USART_pUsartWrite    0xDA
#define USART_pUsartRead     0xDB
#define USART_pUsartOnLine   0xDC
#define USART_CheckLast      0xA1


std::pair<char, char*> MUSART::UsartInit(USART_Type Usart, USARTBAUD_Type Baud,
	USARTWORDLEN_Type WordLength, USARTSTOPBIT_Type StopBits, USARTPARITY_Type Parity,
	USARTHFC_Type HFC, USARTMODE_Type Mode,unsigned char MsgMode) {

	char sendout_[11];
	char judge_ = 0x00;
	sendout_[0] = USART_Check;
	sendout_[1] = USART_pUsartInit;

	sendout_[2] = Usart;
	if (!IS_PARAMS(IS_USART_1234_PERIPH(Usart))) { judge_ = 0x01; return { judge_, sendout_ }; }

	sendout_[3] = Baud;
	if (!IS_PARAMS(IS_USART_BAUD(Baud))) { judge_ = 0x02; return { judge_, sendout_ }; }

	sendout_[4] = WordLength;
	if (!IS_PARAMS(IS_USART_WORD_LENGTH(WordLength))) { judge_ = 0x03; return { judge_, sendout_ }; }

	sendout_[5] = StopBits;
	if (!IS_PARAMS(IS_USART_STOPBITS(StopBits))) { judge_ = 0x04; return { judge_, sendout_ }; }

	sendout_[6] = Parity;
	if (!IS_PARAMS(IS_USART_PARITY(Parity))) { judge_ = 0x05; return { judge_, sendout_ }; }

	sendout_[7] = HFC;
	if (!IS_PARAMS(IS_USART_HARDWARE_FLOW_CONTROL(HFC))) { judge_ = 0x06; return { judge_, sendout_ }; }

	sendout_[8] = Mode;
	if (!IS_PARAMS(IS_USART_MODE(Mode))) { judge_ = 0x07; return { judge_, sendout_ }; }

	sendout_[9] = MsgMode;
	sendout_[10] = USART_CheckLast;

	return { judge_,sendout_ };
}

std::pair<char, char*> MUSART::UsartDeInit(USART_Type Usart) {

	char sendout_[4];
	char judge_ = 0x00;

	sendout_[0] = USART_Check;
	sendout_[1] = USART_pUsartDeInit;

	sendout_[2] = Usart;
	if (!IS_PARAMS(IS_USART_1234_PERIPH(Usart))) { judge_ = 0x01; return { judge_, sendout_ }; }

	sendout_[3] = USART_CheckLast;

	return { judge_,sendout_ };
}

std::pair<char, char*> MUSART::UsartWrite(USART_Type Usart, unsigned char * Words, unsigned char Length) {

	char judge_ = 0x00;
	char params2hex[2];

	memset(UsartWriteBuffer,0,sizeof(UsartWriteBuffer)/sizeof(UsartWriteBuffer[0]));

	*UsartWriteBuffer++ = USART_Check;
	*UsartWriteBuffer++ = USART_pUsartWrite;

	*UsartWriteBuffer++ = Usart;
	if (!IS_PARAMS(IS_USART_1234_PERIPH(Usart))) { judge_ = 0x01; return { judge_, UsartWriteBuffer }; }
	*UsartWriteBuffer++ = Length;

	memcpy(UsartWriteBuffer, Words, Length);
	UsartWriteBuffer++;
	*UsartWriteBuffer = USART_CheckLast;

	return { judge_,UsartWriteBuffer };

}

std::pair<char, char*> MUSART::UsartRead(USART_Type Usart) {

	char sendout_[4];
	char judge_ = 0x00;
	sendout_[0] = USART_Check;
	sendout_[1] = USART_pUsartRead;

	sendout_[2] = Usart;
	if (!IS_PARAMS(IS_USART_1234_PERIPH(Usart))) { judge_ = 0x01; return { judge_, sendout_ }; }

	sendout_[3] = USART_CheckLast;

	return { judge_,sendout_ };

}

char* MUSART::UsartOnLine() {

	char sendout_[3];

	sendout_[0] = USART_Check;
	sendout_[1] = USART_pUsartOnLine;
	sendout_[2] = USART_CheckLast;

	return sendout_;
}




bool MUSART::IS_PARAMS(bool JUDGE) {
	if (JUDGE) return true; else return false;
}

void MUSART::s_To_h(const unsigned short *val, char hex[2]) {

	const char *p = (char*)(val);
	hex[0] = *((char*)p);
	hex[1] = *((char*)(p + 1));

}

