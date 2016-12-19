/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

/**
 * @file
 * @brief This is the application entry point.
 * 			FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 * 			@see L0_LowLevel/lpc_sys.h if you wish to override printf/scanf functions.
 *
 */
#include "tasks.hpp"
#include "examples/examples.hpp"
#include "soft_timer.hpp"
#include "storage.hpp"
#include "i2c2.hpp"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "stdio.h"
#include "io.hpp"
#include "queue.h"
#include "time.h"
#include "printf_lib.h"
#include "LPC17xx.h"
#include "lpc_sys.h"
#include "uart3.hpp"
#include "Final/Alexa.hpp"

//class gpio_task: public scheduler_task {
//public:
//	gpio_task(uint8_t priority) :
//			scheduler_task("gpio", 2000, priority) {
//
//	}
//
//	bool run(void *p) {
//		//LPC_GPIO1->FIOPIN |= (1 << 0);
//		if (LPC_GPIO1->FIOPIN & (1 << 9)) { //onboard LED off
//			LPC_GPIO1->FIOCLR = (1 << 0);
//			vTaskDelay(100);
//		}
//
//		//LPC_GPIO1->FIOPIN &= ~(1 << 0);
//		else /*if (LPC_GPIO1->FIOPIN | (1 << 9))*/{
//			LPC_GPIO1->FIOSET = (1 << 0); // = (1 << 18) | (1 << 23) //onboard LED on
//			vTaskDelay(100);
//		}
//
//		if (LPC_GPIO2->FIOPIN & (1 << 4)) { //external LED on
//			LPC_GPIO2->FIOSET = (1 << 5);
//		}
//
//		else {
//			LPC_GPIO2->FIOCLR = (1 << 5); //external LED off
//		}
//		return true;
//	}
//
//	bool init(void) {
//		printf("init GPIO");
//
//		LPC_PINCON->PINSEL2 &= ~(3 << 0); //bitmask
//
////		LPC_PINCON->PINSEL4 &= ~(1 << 8);
////		LPC_PINCON->PINSEL4 &= ~(1 << 9);
////		LPC_PINCON->PINSEL4 &= ~(1 << 10);
////		LPC_PINCON->PINSEL4 &= ~(1 << 11);
//
//		LPC_PINCON->PINSEL4 &= ~(15 << 8); // Clear bits 8-11
//
//		LPC_GPIO1->FIODIR |= (1 << 0); //on board led
//
//		LPC_GPIO2->FIODIR &= ~(1 << 4); //2.4 input
//		LPC_GPIO2->FIODIR |= (1 << 5); //2.5 output
//
//		return true;
//	}
//};
//
//class spi_task: public scheduler_task {
//public:
//	spi_task(uint8_t priority) :
//			scheduler_task("spi", 2000, priority) {
//
//	}
//
//	bool run(void *p) {
//		LPC_GPIO0->FIOCLR = (1 << 6); //Set CS
//		vTaskDelay(500);
//		spi1_rx_tx(0x9F); //opcode
//		printf("Manufacturer ID: %x\n", spi1_rx_tx(0xFF));
//		printf("Device ID (1): %x\n", spi1_rx_tx(0xFF));
//		printf("Device ID (2): %02x\n\n", spi1_rx_tx(0xFF));
//		LPC_GPIO0->FIOSET = (1 << 6); //Clear CS
//		vTaskDelay(500);
//
//		LPC_GPIO0->FIOCLR = (1 << 6); //Set CS
//		vTaskDelay(500);
//		char byte1 = 'a', byte2 = 'a';
//		spi1_rx_tx(0xd7); //opcode
//		byte1 = spi1_rx_tx(0xFF);
//		byte2 = spi1_rx_tx(0xFF);
//		printf("Ready/Busy Status: %i\n", ((byte1 & 0x80) >> 7));
//		printf("Compare Result: %i\n", ((byte1 & 0x40) >> 6));
//		printf("Density Code: %i%i%i%i\n", ((byte1 & 0x20) >> 5), ((byte1 & 0x10) >> 4), ((byte1 & 0x08) >> 3), ((byte1 & 0x04) >> 2));
//		printf("Sector Protection Status: %i\n", ((byte1 & 0x02) >> 1));
//		printf("Page Size Configuration: %i\n\n", (byte1 & 0x01));
//		printf("Ready/Busy Status: %i\n", ((byte2 & 0x80) >> 7));
//		printf("Reserved for Future Use: %i\n", ((byte2 & 0x40) >> 6));
//		printf("Erase/Program Error: %i\n", ((byte2 & 0x20) >> 5));
//		printf("Reserved for Future Use: %i\n", ((byte2 & 0x10) >> 4));
//		printf("Sector Lockdown Enable: %i\n", ((byte2 & 0x08) >> 3));
//		printf("Program Suspend Status (2): %i\n", ((byte2 & 0x04) >> 2));
//		printf("Program Suspend Status (1): %i\n", ((byte2 & 0x02) >> 1));
//		printf("Erase Suspend: %i\n\n", ((byte1 & 0x01)));
//		LPC_GPIO0->FIOSET = (1 << 6); //Clear CS
//		vTaskDelay(500);
//
//		return true;
//	}
//
//	bool init(void) {
//		LPC_PINCON->PINSEL0 &= ~(0x3 << 12); //Clear 0.6
//		LPC_PINCON->PINSEL0 &= ~(0x3 << 14); //Clear 0.7
//		LPC_PINCON->PINSEL0 &= ~(0x3 << 16); //Clear 0.8
//		LPC_PINCON->PINSEL0 &= ~(0x3 << 18); //Clear 0.9
//
//		LPC_GPIO0->FIODIR |= (1 << 6); //Set 0.6 as input
//
//		BIT(LPC_PINCON->PINSEL0).b15_14 = 2; //SCK enable
//		BIT(LPC_PINCON->PINSEL0).b17_16 = 2; //MISO enable
//		BIT(LPC_PINCON->PINSEL0).b19_18 = 2; //MOSI enable
//
//		LPC_SC->PCONP |= (1 << 10); //SPI1 Power Enable
//		LPC_SC->PCLKSEL0 &= ~(3 << 20); //Clear clock bits
//		BIT(LPC_SC->PCLKSEL0).b21_20 = 1; //48 MHz to SSP1
//
//		LPC_SSP1->CR0 = 7;          //8-bit mode
//		LPC_SSP1->CR1 = (1 << 1);   //Enable SSP as Master
//		LPC_SSP1->CPSR = 2;         //SCK speed = CPU / 2
//
//		return true;
//	}
//
//	char spi1_rx_tx(char send) {
//		LPC_SSP1->DR = send;
//		while (LPC_SSP1->SR & (1 << 4)); // Wait until SSP is busy
//		return LPC_SSP1->DR;
//	}
//};
//
//class uart_task: public scheduler_task {
//public:
//	uart_task(uint8_t priority) :
//			scheduler_task("uart", 2000, priority) {
//
//	}
//
//bool run(void *p) {
//	int choice = 0;
//
//	printf("1 - Send\n 2 - Receive\n");
//	scanf(" %i", &choice);
//
//	if (choice == 1) {
//	printf("\nSending 'H'\n");
//	//vTaskDelay(10);
//	uart3_putchar('H');
//	printf("Sending 'E'\n");
//	//vTaskDelay(10);
//	uart3_putchar('E');
//	printf("Sending 'L'\n");
//	//vTaskDelay(10);
//	uart3_putchar('L');
//	printf("Sending 'L'\n");
//	//vTaskDelay(10);
//	uart3_putchar('L');
//	printf("Sending 'O'\n");
//	//vTaskDelay(10);
//	uart3_putchar('O');
//	//vTaskDelay(500);
//	}
//
//	if (choice == 2) {
//	printf("Received '%c'\n", uart3_getchar());
//	printf("Received '%c'\n", uart3_getchar());
//	printf("Received '%c'\n", uart3_getchar());
//	printf("Received '%c'\n", uart3_getchar());
//	printf("Received '%c'\n", uart3_getchar());
//	//vTaskDelay(500);
//	}
//	return true;
//}
//
//bool init(void) {
//	const uint32_t baud = 9600;
//
//	lpc_pconp(pconp_uart3, true); //Power UART3
//
//	LPC_SC->PCLKSEL1 &= ~(3 << 18); //Clear CLK
//	BIT(LPC_SC->PCLKSEL1).b19_18 = 1; // CLK / 1
//
//	BIT(LPC_UART3->LCR).b7 = 1; //DLAB = 1
//
//	uint16_t quotient = sys_get_cpu_clock() / ((16 * baud) + 0.5); //Rate
//
//	LPC_UART3->DLM = (quotient >> 8); //Shift by 8
//	LPC_UART3->DLL = quotient;
//	LPC_UART3->LCR = 3; //8-bit enable
//
//	LPC_PINCON->PINSEL0 &= ~(0x3 << 0); //Clear 0.0
//	LPC_PINCON->PINSEL0 &= ~(0x3 << 2); //Clear 0.1
//	BIT(LPC_PINCON->PINSEL0).b1_0 = 2; //Enable TXD3
//	BIT(LPC_PINCON->PINSEL0).b3_2 = 2; //Enable RXD3
//
//	return true;
//
////	lpc_pconp(pconp_uart3, true); //Power UART3
////
////	LPC_SC->PCLKSEL1 &= ~(3 << 18); //Clear CLK
////	BIT(LPC_SC->PCLKSEL1).b19_18 = 1; //CLK / 1
////
////	BIT(LPC_UART3->LCR).b7 = 1; //DLAB = 1
////
////	uint16_t dividend = sys_get_cpu_clock() / ((16 * baud) + 0.5);
////	LPC_UART3->DLM = (dividend >> 8);
////	LPC_UART3->DLL = dividend;
////	LPC-UART3->LCR = 3; //8-bit enable
//
////	LPC_PINCON->PINSEL1 &= ~(0x3 << 18); //Clear 0.25
////	LPC_PINCON->PINSEL1 &= ~(0x3 << 20); //Clear 0.26
////	BIT(LPC_PINCON->PINSEL1).b19_18 = 3; //Enable TXD3
////	BIT(LPC_PINCON->PINSEL1).b21_20 = 3; //Enable RXD3
//
////	return true;
//}
//
//void uart3_putchar(char data) {
//	LPC_UART3->THR = data;
//	while(! (LPC_UART3->LSR & (1 << 6)));
//}
//
//char uart3_getchar(void) {
//	while(! (LPC_UART3->LSR & (1 << 0)));
//	return LPC_UART3->RBR;
//}
//
//};
//
//class blah: public scheduler_task {
//public:
//	blah(uint8_t priority) :
//			scheduler_task("blah", 2000, priority) {
//
//	}
//
//	bool run(void *p) {
//		int pinNum = 0;
//
//		printf("Port 2 has been setup to detect interrupts on the rising edge of the clock.\n\nWill you be using pin 2 or 3?\n");
//		scanf(" %i", &pinNum);
//
//		LPC_GPIO2->FIODIR &= ~(1 << pinNum); //Set input pin
//
//
//
//
//
//		if (pinNum == 2) {
//			printf("\nInitializing interrupt service routine for P2.2...\n\n");
//			isr_register(EINT3_IRQn, &gpio_int0);
//			NVIC_EnableIRQ(EINT3_IRQn);
//			while ((LPC_GPIOINT->IO2IntStatR | (0 << 2)) == 0) {
//				u0_dbg_printf("Waiting for interrupt...\n\n");
//				vTaskDelay(2000);
//			}
//
//			//callback(pinNum, &gpio_int0);
//			//vTaskDelay(1000);
//		}
//
//		else if (pinNum == 3) {
//			printf("\nInitializing interrupt service routine for P2.3...\n\n");
//			isr_register(EINT3_IRQn, &gpio_int1);
//			NVIC_EnableIRQ(EINT3_IRQn);
//			while ((LPC_GPIOINT->IO2IntStatR | (0 << 3)) == 0) {
//				u0_dbg_printf("Waiting for interrupt...\n\n");
//				vTaskDelay(2000);
//			}
//
//			//callback(pinNum, &gpio_int1);
//			//vTaskDelay(1000);
//		}
//
//		else {
//			printf("Error! Please choose pin 0 or 1.\n\n");
//			return true;
//		}
//
//		return true;
//	}
//
//	bool init(void) {
//		BIT(LPC_GPIOINT->IO2IntEnR).b2 = 1; //Enable Rising Edge Interrupt on P2.0
//		BIT(LPC_GPIOINT->IO2IntEnR).b3 = 1; //Enable Rising Edge Interrupt on P2.1
////		LPC_PINCON->PINSEL4 |= (3 << 0);
////		LPC_PINCON->PINSEL4 &= ~(3 << 0);
//		return true;
//	}
//
//	static void gpio_int0(void) {
////		while (LPC_GPIOINT->IO2IntStatR & (0 << 0)){
////			u0_dbg_printf("Waiting for interrupt...\n\n");
////			vTaskDelay(2000);
////		}
//
//		u0_dbg_printf("TESTING!!!!!!~!\n");
//
//		if (LPC_GPIOINT->IO2IntStatR & (1 << 2)) {
//			u0_dbg_printf("Interrupt detected on P2.2!\n\nClearing interrupt and restarting...\n\n");
//			LPC_GPIOINT->IO2IntClr |= (1 << 2); //Clear P2.0 Interrupt
//			//vTaskDelay(5000);
//		}
//	}
//
//	static void gpio_int1(void) {
////		while (LPC_GPIOINT->IO2IntStatR & (0 << 1)) {
////			u0_dbg_printf("Waiting for interrupt...\n\n");
////			vTaskDelay(2000);
////		}
//
//		u0_dbg_printf("TESTING!!!!!!~!\n");
//
//		if (LPC_GPIOINT->IO2IntStatR & (1 << 3)) {
//			u0_dbg_printf("Interrupt detected on P2.3!\n\nClearing interrupt and restarting...\n\n");
//			LPC_GPIOINT->IO2IntClr |= (1 << 3); //Clear P2.1 Interrupt
//			//vTaskDelay(5000);
//		}
//	}
//
//	void callback(int pinNum, void (*p)(void)) {
//
//		isr_register(EINT3_IRQn, p); //Send interrupt override
//
//		while (1) {
//			puts("Waiting for interrupt...\n\n");
//			if (LPC_GPIOINT->IO2IntStatR & (1 << 0)) {
//				vTaskDelay(5000);
//				break;
//			}
//
////			else if (LPC_GPIOINT->IO2IntStatR & (1 << 1)) {
////				break;
////			}
//			vTaskDelay(2000);
//		}
//
//	}
//};
//
//class interrupt_task: public scheduler_task {
//public:
//	interrupt_task(uint8_t priority) :
//			scheduler_task("interrupt", 2000, priority) {
//
//	}
//
//	bool run(void *p) {
//		return true;
//	}
//
//	bool init(void) {
//		BIT(LPC_GPIOINT->IO2IntEnR).b0 = 1; //Enable Rising Edge Interrupt on P2.2
//		BIT(LPC_GPIOINT->IO2IntEnR).b1 = 1; //Enable Rising Edge Interrupt on P2.3
//		LPC_GPIO2->FIODIR &= ~(1 << 0);
//		LPC_GPIO2->FIODIR &= ~(1 << 1);
//		isr_register(EINT3_IRQn, &gpio_int);
//		NVIC_EnableIRQ(EINT3_IRQn);
//		return true;
//	}
//
//	static void gpio_int(void) {
//		u0_dbg_printf("Interrupt detected on P2.0!\n");
//		u0_dbg_printf("TEST\n");
//		LPC_GPIOINT->IO2IntClr |= (1 << 0);
//		LPC_GPIOINT->IO2IntClr |= (1 << 1);
//	}
//};
//
//// IDs used for getSharedObject() and addSharedObject()
////typedef enum {
////   shared_SensorQueueId,
////   Lab7SensorQueueId,
////} sharedHandleId_t;
//
///// Orientation type enumeration
//typedef enum {
//    invalid,
//	up,
//	down,
//    left,
//    right,
//} orientation_t;
//
//class orient_compute : public scheduler_task {
//    public:
//        orient_compute(uint8_t priority) : scheduler_task("compute", 2048, priority) {
//            /* We save the queue handle by using addSharedObject() */
//            QueueHandle_t my_queue = xQueueCreate(1, sizeof(orientation_t));
//            addSharedObject(shared_SensorQueueId, my_queue);
//        }
//
//        bool run(void *p) {
//            /* Compute orientation here, and send it to the queue once a second */
//        	puts("Computing the orientation of the SJSUOne Board...\n\n");
//
//        	int x = AS.getX();
//        	int y = AS.getY();
//
//        	if ((x > 350) && (y < 350) && (y > -350)) { //Right Detection
//        		puts("The SJSUOne Board is facing right!\n\n");
//        		orientation_t orientation = right;
//        		xQueueSend(getSharedObject(shared_SensorQueueId), &orientation, portMAX_DELAY);
//        		vTaskDelay(1000);
//        		return true;
//        	}
//
//        	else if ((x < -350) && (y < 350) && (y > -350)) { //Left Detection
//        		puts("The SJSUOne Board is facing left!\n\n");
//        		orientation_t orientation = left;
//				xQueueSend(getSharedObject(shared_SensorQueueId), &orientation, portMAX_DELAY);
//				vTaskDelay(1000);
//				return true;
//			}
//
//        	else if ((y > 350) && (x < 350) && (x > -350)) { //Up Detection
//        		puts("The SJSUOne Board is facing up!\n\n");
//        		orientation_t orientation = up;
//				xQueueSend(getSharedObject(shared_SensorQueueId), &orientation, portMAX_DELAY);
//				vTaskDelay(1000);
//				return true;
//			}
//
//        	else if ((y < -350) && (x < 350) && (x > -350)) { //Down Detection
//        		puts("The SJSUOne Board is facing down!\n\n");
//        		orientation_t orientation = down;
//				xQueueSend(getSharedObject(shared_SensorQueueId), &orientation, portMAX_DELAY);
//				vTaskDelay(1000);
//				return true;
//			}
//
//        	else { //Invalid Detection
//        		puts("The SJSUOne Board is facing an invalid direction!\n\n");
//        		orientation_t orientation = invalid;
//				xQueueSend(getSharedObject(shared_SensorQueueId), &orientation, portMAX_DELAY);
//				vTaskDelay(1000);
//				return true;
//        	}
//
//        }
//};
//
//class orient_process : public scheduler_task {
//    public:
//        orient_process (uint8_t priority) : scheduler_task("process", 2048, priority) {
//            /* Nothing to init */
//        }
//
//        bool run(void *p) {
//            /* We first get the queue handle the other task added using addSharedObject() */
//            orientation_t orientation = invalid;
//            QueueHandle_t qid = getSharedObject(shared_SensorQueueId);
//
//            /* Sleep the task forever until an item is available in the queue */
//            if (xQueueReceive(qid, &orientation, portMAX_DELAY)) {
//            	if (orientation == right) { //Right Detection
//            		LE.on(2);
//            		puts("Queue received the orientation of right!\n\n");
//            		return true;
//            	}
//
//            	else if (orientation == left) { //Left Detection
//            		LE.on(1);
//            		puts("Queue received the orientation of left!\n\n");
//					return true;
//				}
//
//            	else if (orientation == up) { //Up Detection
//            		LE.off(1);
//            		LE.off(2);
//            		puts("Queue received the orientation of up!\n\n");
//            		return true;
//            	}
//
//            	else if (orientation == down) { //Down Detection
//					LE.off(1);
//					LE.off(2);
//					puts("Queue received the orientation of down!\n\n");
//					return true;
//				}
//
//            	else if (orientation == invalid) { //Invalid Detection
//					LE.off(1);
//					LE.off(2);
//					puts("Queue received the orientation of invalid!\n\n");
//					return true;
//				}
//            }
//
//            return true;
//        }
//};
//
//// Create the event group handle used by the three tasks
//EventGroupHandle_t watchdogEventGroup = xEventGroupCreate();
//const uint32_t producerEventBit = (1 << 0); // Bit 0 for producer
//const uint32_t consumerEventBit = (1 << 1); // Bit 1 for consumer
//
//class producer_task: public scheduler_task {
//public:
//	producer_task(uint8_t priority) :
//			scheduler_task("producer", 2048, priority) {
//		// Create queue handle with for light sensor Values
//		QueueHandle_t lab7SensorQueue = xQueueCreate(1, sizeof(float));
//		// Register the queue handle as shared object
//		addSharedObject(Lab7SensorQueueId, lab7SensorQueue);
//	}
//
//	bool run(void *p) {
//		uint16_t total = 0;
//		float lightAvg = 0;
//
//		for (int i = 0; i < 100; i++) {
//			total += LS.getRawValue(); //Get Light Sensor Data
//			vTaskDelay(1);
//		}
//
//		lightAvg = (float) (total / 100); //Calculate Average of 100 Samples
//
//		xQueueSend(getSharedObject(Lab7SensorQueueId), &lightAvg,
//				portMAX_DELAY); //Send to consumer
//
//		xEventGroupSetBits(watchdogEventGroup, producerEventBit); // Set event group bit (task loop complete)
//
//		return true;
//	}
//
//};
//
//class consumer_task: public scheduler_task {
//public:
//	consumer_task(uint8_t priority) :
//			scheduler_task("consumer", 2048, priority) {
//
//	}
//
//	bool run(void *p) {
//		QueueHandle_t qid = getSharedObject(Lab7SensorQueueId);
//		float lightAvg = 0;
//		char charBuffer[21];
//
//		if (xQueueReceive(qid, &lightAvg, portMAX_DELAY)) {
//			time_t clock;
//			struct tm *point;
//			clock = time(NULL);
//			point = localtime(&clock);
//
//			sprintf(charBuffer, "%.2d:%.2d:%.2d, %f\n", point->tm_hour,
//					point->tm_min, point->tm_sec, lightAvg); //Parse data to buffer
//
//			// Save Data to the File
//			Storage::append("1:sensor.txt", &charBuffer, sizeof(charBuffer), 0);
//			vTaskDelay(3000);
//
//		}
//
//		// Set Bit 2 of Event group API
//		xEventGroupSetBits(watchdogEventGroup, consumerEventBit);
//
//		return true;
//	}
//
//};
//
//class watchdog_task: public scheduler_task {
//public:
//	watchdog_task(uint8_t priority) :
//			scheduler_task("watchdog", 2048, priority) {
//
//	}
//
//	bool run(void *p) {
//		// Check Event bits
//		uint32_t result = xEventGroupWaitBits(watchdogEventGroup,
//				(producerEventBit | consumerEventBit),
//				pdTRUE,
//				pdTRUE, 2000);
//
//		if (!(result & (producerEventBit | consumerEventBit))) {
//
//			puts("ERROR: Task 1 and Task 2 are not running properly!\n");
//
//			// Print Stuck.txt
//			char charBuffer[33];
//			sprintf(charBuffer, "ERROR: WatchDog Returned Bits: %x\n", result);
//			Storage::append("1:stuck.txt", &charBuffer, sizeof(charBuffer), 0);
//		}
//
//		// Save System Time
//		time_t clock;
//		struct tm *point;
//		clock = time(NULL);
//		point = localtime(&clock);
//
//		// TODO: Change this to Delay until implementation to make it more "reliable"
//		// Currently watchdog is not called every second.
//		if (point->tm_sec == 0 && !(point->tm_min == 0)) {
//
//			// Print CPU Info File
//			char charBuffer[27];
//			sprintf(charBuffer, "%.2d:%.2d:%.2d, CPU: %d%, Idle: %d%\n",
//					point->tm_hour, point->tm_min, point->tm_sec,
//					getSysCpuPercent(), getSysIdlePercent());
//
//			Storage::append("1:cpu.txt", &charBuffer, sizeof(charBuffer), 0);
//		}
//
//		return true;
//	}
//
//};
//
//class XBEE_task: public scheduler_task {
//public:
//	XBEE_task(uint8_t priority) :
//			scheduler_task("XBEE", 2048, priority) {
//
//	}
//
//	bool run(void *p) {
//		//char buffer[100];
//		char test;
//		Uart3& u3 = Uart3::getInstance();
//		u3.init(9600); /* Init baud rate */
//		u3.putline("Hello World\n");
//		while (1) {
//			u3.getChar(&test);
//			//printf("%c", test);
//		}
//
//		return true;
//	}
//};

/**
 * The main() creates tasks or "threads".  See the documentation of scheduler_task class at scheduler_task.hpp
 * for details.  There is a very simple example towards the beginning of this class's declaration.
 *
 * @warning SPI #1 bus usage notes (interfaced to SD & Flash):
 *      - You can read/write files from multiple tasks because it automatically goes through SPI semaphore.
 *      - If you are going to use the SPI Bus in a FreeRTOS task, you need to use the API at L4_IO/fat/spi_sem.h
 *
 * @warning SPI #0 usage notes (Nordic wireless)
 *      - This bus is more tricky to use because if FreeRTOS is not running, the RIT interrupt may use the bus.
 *      - If FreeRTOS is running, then wireless task may use it.
 *        In either case, you should avoid using this bus or interfacing to external components because
 *        there is no semaphore configured for this bus and it should be used exclusively by nordic wireless.
 */

int main(void)
{
	printf("Test\n");
    /**
     * A few basic tasks for this bare-bone system :
     *      1.  Terminal task provides gateway to interact with the board through UART terminal.
     *      2.  Remote task allows you to use remote control to interact with the board.
     *      3.  Wireless task responsible to receive, retry, and handle mesh network.
     *
     * Disable remote task if you are not using it.  Also, it needs SYS_CFG_ENABLE_TLM
     * such that it can save remote control codes to non-volatile memory.  IR remote
     * control codes can be learned by typing the "learn" terminal command.
     */
    scheduler_add_task(new terminalTask(PRIORITY_HIGH));

    //scheduler_add_task(new gpio_task(PRIORITY_HIGH));

    //scheduler_add_task(new spi_task(PRIORITY_HIGH));

    //scheduler_add_task(new uart_task(PRIORITY_HIGH));

    //scheduler_add_task(new interrupt_task(PRIORITY_HIGH));

    //scheduler_add_task(new orient_compute(PRIORITY_LOW));
    //scheduler_add_task(new orient_process(PRIORITY_LOW));

    //scheduler_add_task(new producer_task(PRIORITY_MEDIUM));
    //scheduler_add_task(new consumer_task(PRIORITY_MEDIUM));
    //scheduler_add_task(new watchdog_task(PRIORITY_HIGH));

    //scheduler_add_task(new XBEE_task(PRIORITY_HIGH));
    scheduler_add_task(new dist_monitoring_consumer_task(PRIORITY_HIGH));
    scheduler_add_task(new dist_monitoring_sensor_task(PRIORITY_LOW));
    scheduler_add_task(new dist_monitoring_pin_task(PRIORITY_LOW));
    scheduler_add_task(new dist_monitoring_os_task(PRIORITY_LOW));

    /* Consumes very little CPU, but need highest priority to handle mesh network ACKs */
    scheduler_add_task(new wirelessTask(PRIORITY_CRITICAL));

    /* Change "#if 0" to "#if 1" to run period tasks; @see period_callbacks.cpp */
    #if 0
    scheduler_add_task(new periodicSchedulerTask());
    #endif

    /* The task for the IR receiver */
    // scheduler_add_task(new remoteTask  (PRIORITY_LOW));

    /* Your tasks should probably used PRIORITY_MEDIUM or PRIORITY_LOW because you want the terminal
     * task to always be responsive so you can poke around in case something goes wrong.
     */

    /**
     * This is a the board demonstration task that can be used to test the board.
     * This also shows you how to send a wireless packets to other boards.
     */
    #if 0
        scheduler_add_task(new example_io_demo());
    #endif

    /**
     * Change "#if 0" to "#if 1" to enable examples.
     * Try these examples one at a time.
     */
    #if 0
        scheduler_add_task(new example_task());
        scheduler_add_task(new example_alarm());
        scheduler_add_task(new example_logger_qset());
        scheduler_add_task(new example_nv_vars());
    #endif

    /**
	 * Try the rx / tx tasks together to see how they queue data to each other.
	 */
    #if 0
        scheduler_add_task(new queue_tx());
        scheduler_add_task(new queue_rx());
    #endif

    /**
     * Another example of shared handles and producer/consumer using a queue.
     * In this example, producer will produce as fast as the consumer can consume.
     */
    #if 0
        scheduler_add_task(new producer());
        scheduler_add_task(new consumer());
    #endif

    /**
     * If you have RN-XV on your board, you can connect to Wifi using this task.
     * This does two things for us:
     *   1.  The task allows us to perform HTTP web requests (@see wifiTask)
     *   2.  Terminal task can accept commands from TCP/IP through Wifly module.
     *
     * To add terminal command channel, add this at terminal.cpp :: taskEntry() function:
     * @code
     *     // Assuming Wifly is on Uart3
     *     addCommandChannel(Uart3::getInstance(), false);
     * @endcode
     */
    #if 0
        Uart3 &u3 = Uart3::getInstance();
        u3.init(WIFI_BAUD_RATE, WIFI_RXQ_SIZE, WIFI_TXQ_SIZE);
        scheduler_add_task(new wifiTask(Uart3::getInstance(), PRIORITY_LOW));
    #endif

    scheduler_start(); ///< This shouldn't return
    return -1;
}
