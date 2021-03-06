#include "stdafx.h"
#include "MIIC.h"

#define IIC1_Port_SCL       0x08
#define IIC1_Port_SDA       0x08
#define IIC1_GPIO_SCL       0x44
#define IIC1_GPIO_SDA       0x48


#define IIC2_Port_SCL       
#define IIC2_Port_SDA       
#define IIC2_GPIO_SCL       
#define IIC2_GPIO_SDA      

#define IIC_pIICInit      0xCA
#define IIC_pIICFunc      0xDA

MIIC::MIIC() {

	sendout_ = (unsigned char*)malloc(60 * sizeof(unsigned char));

}

MIIC::~MIIC() {

	free(sendout_);
}

std::pair<unsigned short, unsigned char*> MIIC::IICInit(IICNUM_Type IIC) {

	IIC_FBInit();
	unsigned short length_ = 8;
	
	
	IIC_RIGHT = IICRIGHT;

	sendout_[0] = IIC_Check;
	sendout_[1] = IIC_pIICInit;

	if (!IS_PARAMS(IS_GET_IIC_NUM(IIC))) { IIC_RIGHT = 0x01; return { length_, sendout_ }; }

	if (IIC == IIC_Num_1) {
	     
		sendout_[2] = IIC1_Port_SCL;
		sendout_[3] = IIC1_GPIO_SCL;
		sendout_[4] = IIC1_Port_SDA;
		sendout_[5] = IIC1_GPIO_SDA;

		
	}else if (IIC == IIC_Num_2) {

		//iic_set[1].IIC_Port_SCL = IIC2_Port_SCL;
		//iic_set[1].IIC_Port_SDA = IIC2_Port_SDA;
		//iic_set[1].IIC_GPIO_SCL = IIC2_GPIO_SCL;
		//iic_set[1].IIC_GPIO_SDA = IIC2_GPIO_SDA;

		//sendout_[2] = IIC2_Port_SCL;
		//sendout_[3] = IIC2_GPIO_SCL;
		//sendout_[4] = IIC2_Port_SDA;
		//sendout_[5] = IIC2_GPIO_SDA;

	}

	short temp = 0;

	for (char i = 2; i < 6; i++) {

		temp += sendout_[i];
	}


	sendout_[6] = temp % 0xFF;
	sendout_[7] = pIICInit__ID;

	return { length_, sendout_ };

}


std::pair<unsigned short,unsigned  char*> MIIC::IICFunc(IICNUM_Type IIC, IICFUNC_Type *Func) {


	IIC_FBInit();

	unsigned char func_length = sizeof(Func) / sizeof(Func[0]);
	unsigned short length_ = 6 + func_length;


	IIC_RIGHT = IICRIGHT;

	sendout_[0] = IIC_Check;
	sendout_[1] = IIC_pIICFunc;

	if (!IS_PARAMS(IS_GET_IIC_NUM(IIC))) { IIC_RIGHT = 0x01; return { length_, sendout_ };}

	sendout_[2]= IIC;


	for (unsigned char i = 0; i < func_length; i++) {

		if (!IS_PARAMS(IS_GET_IIC_FUNC(Func[i]))) { IIC_RIGHT = 0x02; return { length_, sendout_ }; }

		sendout_[3+i] = Func[i];
	}

	sendout_[5 + func_length] = pIICFunc__ID;

	return { length_, sendout_ };

}

std::pair<unsigned short, unsigned  char*> MIIC::MIICSerialFeedBack() {


	unsigned short length_ = 3;


	sendout_[0] = 0xFA;
	sendout_[1] = 0xDC;
	sendout_[2] = 0xA5;

	return { length_, sendout_ };
}

std::pair<unsigned short, unsigned  char*> MIIC::MIICWifiFeedBack(unsigned char ID) {

	unsigned short length_ = 5;


	sendout_[0] = 0xF9;
	sendout_[1] = 0xD5;
	sendout_[2] = 0x01;
	sendout_[3] = ID;
	sendout_[4] = 0x9F;

	return { length_, sendout_ };
}

bool MIIC::IIC_FeedBack(unsigned char ID, unsigned char *Msg) {

	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID)) {
			IIC_ID_CHECK_ = true;
			continue_count++;
		}
		else if (IS_RIGHT(Msg[i], IIC_Check)) {
			IIC_CHECK_ = true;
			continue_count++;
		}
	}

	if (IIC_ID_CHECK_ && IIC_CHECK_ && continue_count == 2) {
		return true;
	}
	return false;

}

bool MIIC::IIC_FeedBack(unsigned char ID, unsigned char *Msg, unsigned char* readback, unsigned char length) {


	unsigned short  msg_length = (unsigned short)(sizeof(Msg) / sizeof(Msg[0]));

	short length_ = 0;

	for (int i = 0; i < msg_length; i++) {

		if (IS_RIGHT(Msg[i], ID) && !IIC_ID_CHECK_) {

			IIC_ID_CHECK_ = true;
			continue_count++;
		}
		else if (continue_count>0) {

			sendout_[continue_count - 1] = Msg[i];
			continue_count++;
		}
		else if (continue_count>1) {
		
			sendout_[continue_count - 1] = Msg[i];
			length_ = Msg[i];
			continue_count++;
		}
		else if (continue_count>2 && continue_count<length_+3) {

			sendout_[continue_count - 1] = Msg[i];
			continue_count++;
		
		}
		else if (continue_count == length_ + 3) {

			unsigned short temp = 0;

			for (char j = 0; j < length_+2; j++) {

				temp += sendout_[j];
			}

			if (Msg[i] == temp % 0xFF) {
				IIC_PARAM_CHECK_ = true;
				continue_count++;
			}

		}
		else if (continue_count == length_ + 4) {
			if (IS_RIGHT(Msg[i], IIC_Check))IIC_CHECK_ = true;
		}
	}

	if (IIC_CHECK_ && IIC_PARAM_CHECK_ && IIC_ID_CHECK_) {
		memcpy(readback, sendout_+2, length_);
		length = length_;
		return true;
	}
	return false;
}


bool MIIC::IS_PARAMS(bool JUDGE) {

	if (JUDGE) return true; else return false;
}

bool MIIC::IS_RIGHT(unsigned char param1, unsigned char param2) {

	if (param1 == param2) {
		return true;
	}
	else {
		return false;
	}

}
