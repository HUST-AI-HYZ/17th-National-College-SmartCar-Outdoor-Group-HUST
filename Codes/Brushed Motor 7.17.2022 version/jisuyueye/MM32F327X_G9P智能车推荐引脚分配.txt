总钻风/小钻风摄像头 UART8 TIM1
	TXD			E0 		<单片机的 UART8 RX>
	RXD			E1 		<单片机的 UART8 TX>
	PLCK		E7 		<单片机的 TIM1_ETR 信号 对应 DMA1_CH4 的 TIM1_TRIG >
	VSYNC		E8
	HREF		MM32采集不需要使用该信号
	DATA		F0-F7

两个电机	TIM5 4路PWM 也可用作2路PWM+2路IO
	A-1/1DIR	A0		<单片机的 TIM5_CH1>
	A-2/1PWM	A1		<单片机的 TIM5_CH2>
	B-1/2DIR	A2		<单片机的 TIM5_CH3>
	B-2/2PWM	A3		<单片机的 TIM5_CH4>

一个舵机	TIM2 1路
	PWM			A15		<单片机的 TIM2_CH1>

两个编码器 两路正交解码 TIM3/4
	编码器1
	A/LSB		B4		<单片机的 TIM3_ENC1>
	B/DIR		B5		<单片机的 TIM3_ENC2>
	编码器2
	A/LSB		B6		<单片机的 TIM4_ENC1>
	B/DIR		B7		<单片机的 TIM4_ENC2>

OLED/TFT1.8/IPS114屏幕 SPI3
	SCL/D0		D4		<单片机的 SPI3 SCK>
	SDA/D1		D6		<单片机的 SPI3 MOSI>
	RST			D13
	DC			D5
	CS			D7
	BL			D11

IPS200屏幕
	RD			D4
	WR			D6
	RS			D13		<单片机的 FMC_A18>
	RST			D5
	CS			D7		<单片机的 FMC_NE1>
	BL			D11
	使用FSMC进行硬件控制
	D0-D4		E11-15	<单片机的 FMC_DA8-FMC_DA12>
	D5-D7		D8-10	<单片机的 FMC_DA13-FMC_DA15>

陀螺仪 ICM20602/MPU6050 SPI2
	SCL/SPC		B13		<单片机的 SPI2 SCK>
	SDA/SDI		B15		<单片机的 SPI2 MOSI>
	SAO/SDO		B14		<单片机的 SPI2 MISO>
	CS			B12

无线转串口模块 UART6
	RX			C6		<单片机的 UART6 TX>
	TX			C7		<单片机的 UART6 RX>
	RTS			C13

红孩儿CCD
	AO			B0		<单片机的 ADC1_CH8>
	AO			B1		<单片机的 ADC1_CH9>
	CLK			B2
	SI			B3

电磁输入
	L1			A4		<单片机的 ADC2_CH4>
	L2			A5		<单片机的 ADC2_CH5>
	L3			A6		<单片机的 ADC2_CH6>
	L4			A7		<单片机的 ADC2_CH7>

	L5			C0		<单片机的 ADC3_CH10>
	L6			C1		<单片机的 ADC3_CH11>
	L7			C2		<单片机的 ADC3_CH12>
	L8			C3		<单片机的 ADC3_CH13>

GPS模块 UART3
	RX 			B10		<单片机的 UART3 TX>
	TX 			B11		<单片机的 UART3 RX>

有来模块 UART3
	RXD			B10		<单片机的 UART3 TX>
	TXD			B11		<单片机的 UART3 RX>
有去模块
	EN			B9

霍尔停车检测模块
	EN			A8
