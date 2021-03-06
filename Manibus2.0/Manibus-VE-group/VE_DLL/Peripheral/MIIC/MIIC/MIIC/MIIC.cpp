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


#define IIC_Check         0xFB
#define IIC_pIICInit      0xCA
#define IIC_pIICFunc      0xDA
#define IIC_CheckLast     0xA1


std::pair<char, char*> MIIC::IICInit(IICNUM_Type IIC) {

	char sendout_[7];
	char judge_ = 0x00;

	sendout_[0] = IIC_Check;
	sendout_[1] = IIC_pIICInit;

	if (!IS_PARAMS(IS_GET_IIC_NUM(IIC))) { judge_ = 0x01; return { judge_, sendout_ }; }

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
	sendout_[6] = IIC_CheckLast;

	return { judge_, sendout_ };

}


std::pair<char, char*> MIIC::IICFunc(IICNUM_Type IIC, IICFUNC_Type *Func) {

	char judge_ = 0x00;

	memset(IIC_OrderBuffer, 0, sizeof(IIC_OrderBuffer) / sizeof(IIC_OrderBuffer[0]));

	*IIC_OrderBuffer++ = IIC_Check;
	*IIC_OrderBuffer++ = IIC_pIICFunc;

	if (!IS_PARAMS(IS_GET_IIC_NUM(IIC))) { judge_ = 0x01; return { judge_, IIC_OrderBuffer };}

	*IIC_OrderBuffer++ = IIC;

	unsigned char length = sizeof(Func) / sizeof(Func[0]);

	for (unsigned char i = 0; i < length; i++) {

		if (!IS_PARAMS(IS_GET_IIC_FUNC(Func[i]))) { judge_ = 0x02; return { judge_, IIC_OrderBuffer }; }

		*IIC_OrderBuffer++ = Func[i];
	}

	*IIC_OrderBuffer = IIC_CheckLast;

	return { judge_, IIC_OrderBuffer };

}

bool MIIC::IS_PARAMS(bool JUDGE) {

	if (JUDGE) return true; else return false;
}

