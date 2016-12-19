#include "Alexa.hpp"
#include "stdio.h"
#include "LPC17xx.h"
#include "L4_IO/fat/disk/spi_flash.h"
#include "tasks.hpp"
#include "io.hpp"
#include "event_groups.h"
#include "time.h"
#include "queue.h"
#include "uart3.hpp"
#include "switches.hpp"

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"               // uxTaskGetSystemState()

#include "command_handler.hpp"  // CMD_HANDLER_FUNC()
#include "rtc.h"                // Set and Get System Time
#include "sys_config.h"         // TERMINAL_END_CHARS
#include "lpc_sys.h"

#include "utilities.h"          // printMemoryInfo()
#include "storage.hpp"          // Get Storage Device instances
#include "fat/disk/spi_flash.h"

/*
 * Final Project Tasks
 */

dist_monitoring_consumer_task::dist_monitoring_consumer_task(uint8_t priority) :
		scheduler_task("dist_monitoring_consumer_task", 2048, priority) {
}

bool dist_monitoring_consumer_task::init(void) {
	return true;
}

bool dist_monitoring_consumer_task::run(void* p) {
	sensorData sensor;
	pinData pin;
	osData os;
	xQueueReceive(getSharedObject(DistMonitoringSensorQueueId), &sensor, portMAX_DELAY);
	xQueueReceive(getSharedObject(DistMonitoringPinQueueId), &pin, portMAX_DELAY);
	xQueueReceive(getSharedObject(DistMonitoringOsQueueId), &os, portMAX_DELAY);

	printf("---------------------------------------------\n");
	vTaskDelay(100);
	printf("temp: %0.2f\n", sensor.temp);
	vTaskDelay(1000);
	printf("light: %u%%\n", sensor.light);
	vTaskDelay(1000);
	printf("x: %i\n", sensor.x);
	vTaskDelay(1000);
	printf("y: %i\n", sensor.y);
	vTaskDelay(1000);
	printf("sw1: %i\n", pin.sw1);
	vTaskDelay(1000);
	printf("sw2: %i\n", pin.sw2);
	vTaskDelay(1000);
	printf("sw3: %i\n", pin.sw3);
	vTaskDelay(1000);
	printf("sw4: %i\n", pin.sw4);
	vTaskDelay(1000);

	for (unsigned int i = 0; i < os.count; i++) {
		printf("taskName: %15s\n", os.taskName[i]);
		vTaskDelay(1000);
		printf("cpuPercent: %4u\n", os.cpuPercent[i]);
		vTaskDelay(1000);
	}

	printf("globalUsed: %5d\n", os.globalUsed);
	vTaskDelay(1000);
	printf("mallocUsed: %5d\n", os.mallocUsed);
	vTaskDelay(1000);
	printf("mallocAvail: %5d\n", os.mallocAvail);
	vTaskDelay(1000);
	printf("systemAvail: %5d\n", os.systemAvail);
	vTaskDelay(1000);
	printf("---------------------------------------------\n");



//	Uart3& u3 = Uart3::getInstance();
//
//	u3.init(9600); /* Init baud rate */
//
//	u3.putline("Hello World\n");
//
//	if (u3.isReady()) {
//		printf("uart3 ready!");
//	}
//	uint16_t lightVal;
//	while (1) {
//		lightVal = LS.getRawValue();
//		u3.printf("%d\n", lightVal);
//		vTaskDelay(100);
//
//	}

	return true;
}

dist_monitoring_sensor_task::dist_monitoring_sensor_task(uint8_t priority) :
		scheduler_task("dist_monitoring_sensor_task", 2048, priority) {
}

bool dist_monitoring_sensor_task::init(void) {
	// Create & register queue handle
	QueueHandle_t distMonitoringSensorQueue = xQueueCreate(1, sizeof(sensorData));
	addSharedObject(DistMonitoringSensorQueueId, distMonitoringSensorQueue);
	return true;
}

bool dist_monitoring_sensor_task::run(void* p) {
	sensorData data;
	data.temp = TS.getFarenheit();
	data.light = LS.getPercentValue();
	data.x = AS.getX();
	data.y = AS.getY();
	xQueueSend(getSharedObject(DistMonitoringSensorQueueId), &data, portMAX_DELAY);
	return true;
}

dist_monitoring_pin_task::dist_monitoring_pin_task(uint8_t priority) :
		scheduler_task("dist_monitoring_pin_task", 2048, priority) {
}

bool dist_monitoring_pin_task::init(void) {
	// Create & register queue handle
	QueueHandle_t distMonitoringPinQueue = xQueueCreate(1, sizeof(pinData));
	addSharedObject(DistMonitoringPinQueueId, distMonitoringPinQueue);

	return true;
}

bool dist_monitoring_pin_task::run(void* p) {
//Button
	pinData data;
	data.sw1 = SW.getSwitch(1);
	data.sw2 = SW.getSwitch(2);
	data.sw3 = SW.getSwitch(3);
	data.sw4 = SW.getSwitch(4);
	xQueueSend(getSharedObject(DistMonitoringPinQueueId), &data, portMAX_DELAY);
	return true;
}

dist_monitoring_os_task::dist_monitoring_os_task(uint8_t priority) :
		scheduler_task("dist_monitoring_os_task", 2048, priority) {
}

bool dist_monitoring_os_task::init(void) {
	// Create & register queue handle
	QueueHandle_t distMonitoringOsQueue = xQueueCreate(1, sizeof(osData));
	addSharedObject(DistMonitoringOsQueueId, distMonitoringOsQueue);

	return true;
}

bool dist_monitoring_os_task::run(void* p) {
	osData data;
//CPU

    // Limit the tasks to avoid heap allocation.
    const unsigned portBASE_TYPE maxTasks = 16;
    TaskStatus_t status[maxTasks];
    uint32_t totalRunTime = 0;
    const unsigned portBASE_TYPE uxArraySize =
            uxTaskGetSystemState(&status[0], maxTasks, &totalRunTime);
    data.count = uxArraySize;

    for(unsigned priorityNum = 0; priorityNum < configMAX_PRIORITIES; priorityNum++)
    {
        /* Print in sorted priority order */
        for (unsigned i = 0; i < uxArraySize; i++) {
            TaskStatus_t *e = &status[i];
            if (e->uxBasePriority == priorityNum) {
            	data.taskName[i] = e->pcTaskName;
            	data.cpuPercent[i] = (0 == totalRunTime) ? 0 : e->ulRunTimeCounter / (totalRunTime/100);
            }
        }
    }

//Mem
    sys_mem_t mem = sys_get_mem_info();
    data.globalUsed = mem.used_global;
    data.mallocUsed = mem.used_heap;
    data.mallocAvail = mem.avail_heap;
    data.systemAvail = mem.avail_sys;

    xQueueSend(getSharedObject(DistMonitoringOsQueueId), &data, portMAX_DELAY);
	return true;
}

/*
 * End Final Project Tasks
 */
