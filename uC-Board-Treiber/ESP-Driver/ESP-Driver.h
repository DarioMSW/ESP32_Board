/*********************************************************************************\
*
* MMMMMMMMMMMM   SSSSSSSSSSSS   WW   WW   WW   MECHATRONIK
* MM   MM   MM   SS             WW   WW   WW   SCHULE
* MM   MM   MM   SSSSSSSSSSSS   WW   WW   WW   WINTERTHUR
* MM   MM   MM             SS   WW   WW   WW   
* MM   MM   MM   SSSSSSSSSSSS   WWWWWWWWWWWW   www.msw.ch
*
*
* Dateiname: ESP32_Board_Treiber.h
*
* Projekt  : ESP32 Board Treiber 
* Hardware : ESP32-S3
*
* Copyright: MSW, E4
*
* Beschreibung:
* =============
* Treiber für das ESP32 Board
*
* Portbelegung:
* =============
* Siehe Hardwarestruktur
*
* Verlauf:
* ========
* Datum:      Autor:         Version   Grund der Änderung:
* 23.08.2024  S. Huruvarshan V1.0      Neuerstellung
*
\*********************************************************************************/
#ifndef ESP_DRIVER_H
#define ESP_DRIVER_H

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h> 
#include <driver/gpio.h> 
//#include <driver/i2c.h>
#include <driver/i2c_master.h>

void initBoard(uint8_t startAnimation);
void initI2C (void); 
void mcp23017WriteRegister(uint8_t address, uint8_t register, uint8_t data);
void mcp23017InitIC2(void);
void mcp23017InitIC3(void); 
void ledWriteAll(uint16_t bitMask); 
uint8_t mcp23017ReadRegister(uint8_t address, uint8_t reg); 
uint8_t swtichReadAll(); 
uint8_t buttonReadAll(); 

#endif // ESP_DRIVER_H