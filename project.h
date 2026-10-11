										   /*=========================================================
    project.h
    SMART WEATHER MONITORING SYSTEM
    LPC1768 ARM Cortex-M3
=========================================================*/

#ifndef PROJECT_H
#define PROJECT_H

#include <LPC17xx.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*=========================================================
                    THRESHOLDS
=========================================================*/

#define TEMP_THRESHOLD     20
#define MQ2_THRESHOLD      300

/*=========================================================
                    LCD PINS
=========================================================*/

#define LCD_RS     (1<<10)      // P0.10
#define LCD_EN     (1<<11)      // P0.11

/* LCD DATA : P0.15 - P0.22 */

/*=========================================================
                  SENSOR CONNECTIONS
=========================================================*/

/* DHT11 */
#define DHT11_PIN      (1<<4)       // P0.4

/* Rain Sensor */
#define RAIN_PIN       (1<<23)      // P0.23

/* LDR Sensor */
#define LDR_PIN        (1<<29)      // P1.29

/*=========================================================
                    ADC CHANNELS
=========================================================*/

/* LM35 */
#define LM35_CHANNEL   2           // AD0.2 P0.25

/* MQ2 */
#define MQ2_CHANNEL    3           // AD0.3 P0.26

/*=========================================================
                  ALERT OUTPUTS
=========================================================*/

/* External LEDs */
#define GREEN_LED      (1<<0)      // P0.0
#define ORANGE_LED     (1<<1)      // P0.1
#define RED_LED        (1<<5)      // P0.5

/* Onboard LEDs */
#define LED1           (1<<22)     // P1.22
#define LED2           (1<<23)     // P1.23
#define LED3           (1<<24)     // P1.24

/* Buzzer */
#define BUZZER         (1<<27)     // P1.27

/* Relay */
#define RELAY          (1<<28)     // P1.28

/*=========================================================
                MAX7219 CONNECTIONS
=========================================================*/

#define MAX_CS         (1<<6)      // P0.6

/* SSP1
   SCK  -> P0.7
   MISO -> P0.8
   MOSI -> P0.9
*/

/*=========================================================
                    KEYPAD
=========================================================*/

/* Rows */
#define R1             (1<<0)      // P2.0
#define R2             (1<<1)      // P2.1
#define R3             (1<<2)      // P2.2
#define R4             (1<<3)      // P2.3

/* Columns */
#define C1             (1<<4)      // P2.4
#define C2             (1<<5)      // P2.5
#define C3             (1<<6)      // P2.6
#define C4             (1<<7)      // P2.7

/*=========================================================
                GLOBAL VARIABLES
=========================================================*/

extern uint8_t dht_temp;
extern uint8_t dht_hum;

extern uint16_t lm35_value;
extern uint16_t mq2_value;

extern uint8_t rain_status;
extern uint8_t ldr_status;

extern uint8_t fan_status;
extern uint8_t buzzer_status;

/*=========================================================
                FUNCTION PROTOTYPES
=========================================================*/

/* Delay */
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

/* LCD */
void LCD_Init(void);
void LCD_Command(unsigned char cmd);
void LCD_Data(unsigned char data);
void LCD_String(char *str);
void LCD_Clear(void);
void LCD_Goto(uint8_t row,uint8_t col);

/* ADC */
void ADC_Init(void);
uint16_t ADC_Read(uint8_t channel);

/* DHT11 */
void DHT11_Start(void);
uint8_t DHT11_CheckResponse(void);
uint8_t DHT11_ReadByte(void);
void DHT11_Read(void);

/* Keypad */
void Keypad_Init(void);
char Keypad_Scan(void);

/* MAX7219 */
void MAX7219_Init(void);
void MAX7219_Write(uint16_t data);
void MAX7219_Display(uint16_t value);

/* Alerts */
void Normal_Condition(void);
void Temperature_Alert(void);
void MQ2_Alert(void);
void Rain_Alert(void);

/* Sensor Functions */
uint16_t Read_LM35(void);
uint16_t Read_MQ2(void);
uint8_t Read_Rain(void);
uint8_t Read_LDR(void);

/* Display Functions */
void Display_DHT_Temp(void);
void Display_DHT_Humidity(void);
void Display_LM35(void);
void Display_MQ2(void);
void Display_Rain(void);
void Display_LDR(void);
void Display_Fan(void);
void Display_Buzzer(void);
void Display_Project(void);
void Display_MainScreen(void);

/* Control */
void Check_Conditions(void);

#endif

/*=================== END OF FILE ===================*/