/*=========================================================
                    functions.c
                    PART 2A
        Delay + LCD + ADC Functions
=========================================================*/

#include "project.h"

/*=========================================================
                GLOBAL VARIABLES
=========================================================*/

uint8_t dht_temp = 0;
uint8_t dht_hum  = 0;

uint16_t lm35_value = 0;
uint16_t mq2_value  = 0;

uint8_t rain_status = 0;
uint8_t ldr_status  = 0;

uint8_t fan_status    = 0;
uint8_t buzzer_status = 0;

/*=========================================================
                    DELAY FUNCTIONS
=========================================================*/

void delay_ms(uint32_t ms)
{
    uint32_t i,j;

    for(i=0;i<ms;i++)
    {
        for(j=0;j<1200;j++);
    }
}

void delay_us(uint32_t us)
{
    uint32_t i,j;

    for(i=0;i<us;i++)
    {
        for(j=0;j<12;j++);
    }
}

/*=========================================================
                    LCD FUNCTIONS
=========================================================*/

void LCD_Command(unsigned char cmd)
{
    LPC_GPIO0->FIOCLR = (0xFF << 15);

    LPC_GPIO0->FIOSET = ((uint32_t)cmd << 15);

    LPC_GPIO0->FIOCLR = LCD_RS;

    LPC_GPIO0->FIOSET = LCD_EN;
    delay_ms(2);
    LPC_GPIO0->FIOCLR = LCD_EN;
}

void LCD_Data(unsigned char data)
{
    LPC_GPIO0->FIOCLR = (0xFF << 15);

    LPC_GPIO0->FIOSET = ((uint32_t)data << 15);

    LPC_GPIO0->FIOSET = LCD_RS;

    LPC_GPIO0->FIOSET = LCD_EN;
    delay_ms(2);
    LPC_GPIO0->FIOCLR = LCD_EN;
}

void LCD_String(char *str)
{
    while(*str)
    {
        LCD_Data(*str++);
    }
}

void LCD_Clear(void)
{
    LCD_Command(0x01);
    delay_ms(5);
}

void LCD_Goto(uint8_t row,uint8_t col)
{
    uint8_t pos;

    if(row==0)
        pos = 0x80 + col;
    else
        pos = 0xC0 + col;

    LCD_Command(pos);
}

void LCD_Init(void)
{
    LPC_GPIO0->FIODIR |= LCD_RS;
    LPC_GPIO0->FIODIR |= LCD_EN;

    LPC_GPIO0->FIODIR |= (0xFF << 15);

    delay_ms(20);

    LCD_Command(0x38);      /* 8-bit */
    LCD_Command(0x0C);      /* Display ON */
    LCD_Command(0x06);      /* Entry Mode */
    LCD_Command(0x01);      /* Clear */

    delay_ms(5);
}

/*=========================================================
                    ADC FUNCTIONS
=========================================================*/

void ADC_Init(void)
{
    /* Power ADC */

    LPC_SC->PCONP |= (1<<12);

    /* PCLK = CCLK/8 */

    LPC_SC->PCLKSEL0 &= ~(3<<24);

    /* AD0.2 -> P0.25 */

    LPC_PINCON->PINSEL1 &= ~(3<<18);
    LPC_PINCON->PINSEL1 |=  (1<<18);

    /* AD0.3 -> P0.26 */

    LPC_PINCON->PINSEL1 &= ~(3<<20);
    LPC_PINCON->PINSEL1 |=  (1<<20);

    /* ADC Control */

    LPC_ADC->ADCR =
            (1<<21) |      /* Enable ADC */
            (4<<8);        /* ADC Clock Divider */
}

uint16_t ADC_Read(uint8_t channel)
{
    uint16_t result;

    LPC_ADC->ADCR &= ~(0xFF);

    LPC_ADC->ADCR |= (1<<channel);

    LPC_ADC->ADCR |= (1<<24);

    while(!(LPC_ADC->ADGDR & (1UL<<31)));

    result = (LPC_ADC->ADGDR >> 4) & 0x0FFF;

    LPC_ADC->ADCR &= ~(1<<24);

    return result;
}

/*=========================================================
            LM35 TEMPERATURE FUNCTION
=========================================================*/

uint16_t Read_LM35(void)
{
    uint32_t adc;

    adc = ADC_Read(LM35_CHANNEL);

    /* Convert ADC to Temperature */

    adc = (adc * 3300) / 4095;

    adc = adc / 10;

    lm35_value = adc;

    return lm35_value;
}

/*=========================================================
                MQ2 SENSOR FUNCTION
=========================================================*/

uint16_t Read_MQ2(void)
{
    mq2_value = ADC_Read(MQ2_CHANNEL);

    return mq2_value;
}

/*=========================================================
                RAIN SENSOR FUNCTION
=========================================================*/

uint8_t Read_Rain(void)
{
    if(LPC_GPIO0->FIOPIN & RAIN_PIN)
        rain_status = 0;
    else
        rain_status = 1;

    return rain_status;
}

/*=========================================================
                LDR SENSOR FUNCTION
=========================================================*/

uint8_t Read_LDR(void)
{
    if(LPC_GPIO1->FIOPIN & LDR_PIN)
        ldr_status = 1;
    else
        ldr_status = 0;

    return ldr_status;
}

/*==================== END PART 2A ====================*/

/*=========================================================
                    functions.c
                    PART 2B
            DHT11 + KEYPAD FUNCTIONS
=========================================================*/

#include "project.h"

/*=========================================================
                    DHT11 FUNCTIONS
=========================================================*/

void DHT11_Start(void)
{
    /* Configure P0.4 as Output */

    LPC_GPIO0->FIODIR |= DHT11_PIN;

    /* Send Start Signal */

    LPC_GPIO0->FIOCLR = DHT11_PIN;

    delay_ms(20);

    LPC_GPIO0->FIOSET = DHT11_PIN;

    delay_us(30);

    /* Configure as Input */

    LPC_GPIO0->FIODIR &= ~DHT11_PIN;
}

uint8_t DHT11_CheckResponse(void)
{
    uint8_t response = 0;

    delay_us(40);

    if(!(LPC_GPIO0->FIOPIN & DHT11_PIN))
    {
        delay_us(80);

        if(LPC_GPIO0->FIOPIN & DHT11_PIN)
        {
            response = 1;
        }
    }

    while(LPC_GPIO0->FIOPIN & DHT11_PIN);

    return response;
}

uint8_t DHT11_ReadByte(void)
{
    uint8_t i;
    uint8_t data = 0;

    for(i=0;i<8;i++)
    {
        while(!(LPC_GPIO0->FIOPIN & DHT11_PIN));

        delay_us(40);

        if(LPC_GPIO0->FIOPIN & DHT11_PIN)
        {
            data |= (1 << (7-i));
        }

        while(LPC_GPIO0->FIOPIN & DHT11_PIN);
    }

    return data;
}

void DHT11_Read(void)
{
    uint8_t RH_Int;
    uint8_t RH_Dec;
    uint8_t Temp_Int;
    uint8_t Temp_Dec;
    uint8_t CheckSum;

    DHT11_Start();

    if(DHT11_CheckResponse())
    {
        RH_Int   = DHT11_ReadByte();
        RH_Dec   = DHT11_ReadByte();

        Temp_Int = DHT11_ReadByte();
        Temp_Dec = DHT11_ReadByte();

        CheckSum = DHT11_ReadByte();

        if((RH_Int + RH_Dec + Temp_Int + Temp_Dec) == CheckSum)
        {
            dht_temp = Temp_Int;
            dht_hum  = RH_Int;
        }
    }
}

/*=========================================================
                    KEYPAD FUNCTIONS
=========================================================*/

void Keypad_Init(void)
{
    /* Rows Output */

    LPC_GPIO2->FIODIR |= R1;
    LPC_GPIO2->FIODIR |= R2;
    LPC_GPIO2->FIODIR |= R3;
    LPC_GPIO2->FIODIR |= R4;

    /* Columns Input */

    LPC_GPIO2->FIODIR &= ~C1;
    LPC_GPIO2->FIODIR &= ~C2;
    LPC_GPIO2->FIODIR &= ~C3;
    LPC_GPIO2->FIODIR &= ~C4;
}

char Keypad_Scan(void)
{
    /*---------------- ROW 1 ----------------*/

    LPC_GPIO2->FIOSET = R1|R2|R3|R4;
    LPC_GPIO2->FIOCLR = R1;

    if(!(LPC_GPIO2->FIOPIN & C1))
    {
        delay_ms(20);
        return '1';
    }

    if(!(LPC_GPIO2->FIOPIN & C2))
    {
        delay_ms(20);
        return '2';
    }

    if(!(LPC_GPIO2->FIOPIN & C3))
    {
        delay_ms(20);
        return '3';
    }

    if(!(LPC_GPIO2->FIOPIN & C4))
    {
        delay_ms(20);
        return 'A';
    }

    /*---------------- ROW 2 ----------------*/

    LPC_GPIO2->FIOSET = R1|R2|R3|R4;
    LPC_GPIO2->FIOCLR = R2;

    if(!(LPC_GPIO2->FIOPIN & C1))
    {
        delay_ms(20);
        return '4';
    }

    if(!(LPC_GPIO2->FIOPIN & C2))
    {
        delay_ms(20);
        return '5';
    }

    if(!(LPC_GPIO2->FIOPIN & C3))
    {
        delay_ms(20);
        return '6';
    }

    if(!(LPC_GPIO2->FIOPIN & C4))
    {
        delay_ms(20);
        return 'B';
    }

    /*---------------- ROW 3 ----------------*/

    LPC_GPIO2->FIOSET = R1|R2|R3|R4;
    LPC_GPIO2->FIOCLR = R3;

    if(!(LPC_GPIO2->FIOPIN & C1))
    {
        delay_ms(20);
        return '7';
    }

    if(!(LPC_GPIO2->FIOPIN & C2))
    {
        delay_ms(20);
        return '8';
    }

    if(!(LPC_GPIO2->FIOPIN & C3))
    {
        delay_ms(20);
        return '9';
    }

    if(!(LPC_GPIO2->FIOPIN & C4))
    {
        delay_ms(20);
        return 'C';
    }

    /*---------------- ROW 4 ----------------*/

    LPC_GPIO2->FIOSET = R1|R2|R3|R4;
    LPC_GPIO2->FIOCLR = R4;

    if(!(LPC_GPIO2->FIOPIN & C1))
    {
        delay_ms(20);
        return '*';
    }

    if(!(LPC_GPIO2->FIOPIN & C2))
    {
        delay_ms(20);
        return '0';
    }

    if(!(LPC_GPIO2->FIOPIN & C3))
    {
        delay_ms(20);
        return '#';
    }

    if(!(LPC_GPIO2->FIOPIN & C4))
    {
        delay_ms(20);
        return 'D';
    }

    return 0;
}

/*==================== END PART 2B ====================*/


/*=========================================================
                    functions.c
                    PART 2C
      MAX7219 + ALERTS + DISPLAY FUNCTIONS
=========================================================*/

#include "project.h"

/*=========================================================
                MAX7219 FUNCTIONS
=========================================================*/

void MAX7219_Write(uint16_t data)
{
    LPC_GPIO0->FIOCLR = MAX_CS;

    while(!(LPC_SSP1->SR & (1<<1)));

    LPC_SSP1->DR = data;

    while(LPC_SSP1->SR & (1<<4));

    LPC_GPIO0->FIOSET = MAX_CS;
}

void MAX7219_Init(void)
{
    LPC_SC->PCONP |= (1<<10);

    LPC_PINCON->PINSEL0 |= (2<<14);
    LPC_PINCON->PINSEL0 |= (2<<16);
    LPC_PINCON->PINSEL0 |= (2<<18);

    LPC_GPIO0->FIODIR |= MAX_CS;
    LPC_GPIO0->FIOSET = MAX_CS;

    LPC_SSP1->CR0 = 0x0B;
    LPC_SSP1->CPSR = 20;
    LPC_SSP1->CR1 = (1<<1);

    MAX7219_Write(0x0C01);
    MAX7219_Write(0x0A0F);
    MAX7219_Write(0x0B03);
    MAX7219_Write(0x090F);
}

void MAX7219_Display(uint16_t value)
{
    MAX7219_Write(0x01 | ((value%10)<<8));
    MAX7219_Write(0x02 | (((value/10)%10)<<8));
    MAX7219_Write(0x03 | (((value/100)%10)<<8));
    MAX7219_Write(0x04 | (((value/1000)%10)<<8));
}

/*=========================================================
                ALERT FUNCTIONS
=========================================================*/

void Normal_Condition(void)
{
    LPC_GPIO0->FIOSET = GREEN_LED;
    LPC_GPIO1->FIOSET = LED1;

    LPC_GPIO0->FIOCLR = ORANGE_LED | RED_LED;
    LPC_GPIO1->FIOCLR = LED2 | LED3;

    LPC_GPIO1->FIOCLR = BUZZER;
    LPC_GPIO1->FIOCLR = RELAY;

    fan_status = 0;
    buzzer_status = 0;
}

void Temperature_Alert(void)
{
    LPC_GPIO0->FIOSET = RED_LED;
    LPC_GPIO1->FIOSET = LED3;

    LPC_GPIO0->FIOCLR = GREEN_LED | ORANGE_LED;
    LPC_GPIO1->FIOCLR = LED1 | LED2;

    LPC_GPIO1->FIOSET = BUZZER;
    LPC_GPIO1->FIOSET = RELAY;

    fan_status = 1;
    buzzer_status = 1;
}

void MQ2_Alert(void)
{
    LPC_GPIO0->FIOSET = ORANGE_LED;
    LPC_GPIO1->FIOSET = LED2;

    LPC_GPIO1->FIOSET = BUZZER;

    buzzer_status = 1;
}

void Rain_Alert(void)
{
    LPC_GPIO0->FIOSET = ORANGE_LED;
    LPC_GPIO1->FIOSET = LED2;
}

/*=========================================================
                CONDITION CHECK
=========================================================*/

void Check_Conditions(void)
{
    Read_LM35();
    Read_MQ2();
    Read_Rain();

    if(lm35_value > TEMP_THRESHOLD)
    {
        Temperature_Alert();
    }
    else if(mq2_value > MQ2_THRESHOLD)
    {
        MQ2_Alert();
    }
    else if(rain_status == 1)
    {
        Rain_Alert();
    }
    else
    {
        Normal_Condition();
    }
}

/*=========================================================
                LCD DISPLAY FUNCTIONS
=========================================================*/

void Display_DHT_Temp(void)
{
    char buf[16];

    LCD_Clear();

    LCD_String("DHT TEMP");

    LCD_Goto(1,0);

    sprintf(buf,"TEMP=%d C",dht_temp);

    LCD_String(buf);

    MAX7219_Display(dht_temp);
}

void Display_DHT_Humidity(void)
{
    char buf[16];

    LCD_Clear();

    LCD_String("HUMIDITY");

    LCD_Goto(1,0);

    sprintf(buf,"HUM=%d %%",dht_hum);

    LCD_String(buf);

    MAX7219_Display(dht_hum);
}

void Display_LM35(void)
{
    char buf[16];

    LCD_Clear();

    LCD_String("LM35 TEMP");

    LCD_Goto(1,0);

    sprintf(buf,"TEMP=%d C",lm35_value);

    LCD_String(buf);

    MAX7219_Display(lm35_value);
}

void Display_MQ2(void)
{
    char buf[16];

    LCD_Clear();

    LCD_String("MQ2 SENSOR");

    LCD_Goto(1,0);

    sprintf(buf,"GAS=%d",mq2_value);

    LCD_String(buf);

    MAX7219_Display(mq2_value);
}

void Display_Rain(void)
{
    LCD_Clear();

    LCD_String("RAIN SENSOR");

    LCD_Goto(1,0);

    if(rain_status)
    {
        LCD_String("RAIN DETECTED");
        MAX7219_Display(1);
    }
    else
    {
        LCD_String("NO RAIN");
        MAX7219_Display(0);
    }
}

void Display_LDR(void)
{
    LCD_Clear();

    LCD_String("LDR SENSOR");

    LCD_Goto(1,0);

    if(ldr_status)
    {
        LCD_String("DARK");
        MAX7219_Display(1);
    }
    else
    {
        LCD_String("BRIGHT");
        MAX7219_Display(0);
    }
}

void Display_Fan(void)
{
    LCD_Clear();

    LCD_String("FAN STATUS");

    LCD_Goto(1,0);

    if(fan_status)
    {
        LCD_String("FAN ON");
        MAX7219_Display(1);
    }
    else
    {
        LCD_String("FAN OFF");
        MAX7219_Display(0);
    }
}

void Display_Buzzer(void)
{
    LCD_Clear();

    LCD_String("BUZZER STATUS");

    LCD_Goto(1,0);

    if(buzzer_status)
    {
        LCD_String("BUZZER ON");
        MAX7219_Display(1);
    }
    else
    {
        LCD_String("BUZZER OFF");
        MAX7219_Display(0);
    }
}

void Display_Project(void)
{
    LCD_Clear();

    LCD_String("LPC1768 BASED");

    LCD_Goto(1,0);

    LCD_String("ENV MONITOR");

    MAX7219_Display(1768);
}

void Display_MainScreen(void)
{
    LCD_Clear();

    LCD_String("SMART WEATHER");

    LCD_Goto(1,0);

    LCD_String("SYSTEM NORMAL");

    MAX7219_Display(0);
}

/*==================== END PART 2C ====================*/