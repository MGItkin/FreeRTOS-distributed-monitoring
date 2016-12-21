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
dist_monitoring_data_fresh_task::dist_monitoring_data_fresh_task(uint8_t priority) :
		scheduler_task("dm_data_fresh", 2048, priority) {
}
bool dist_monitoring_data_fresh_task::init(void){
	return true;
}

bool dist_monitoring_data_fresh_task::run(void* p){
	sensorData sensor;
	pinData pin;
	osData os;
	xQueueReceive(getSharedObject(DistMonitoringSensorQueueId), &sensor, portMAX_DELAY);
	xQueueReceive(getSharedObject(DistMonitoringPinQueueId), &pin, portMAX_DELAY);
	xQueueReceive(getSharedObject(DistMonitoringOsQueueId), &os, portMAX_DELAY);

	// data collection refreshed every 50 ms
	// NOTE: that is needed specifically for OS data collection.
	//       with no delay, OS data collection took around 48% CPU utilization
	vTaskDelay(50);

	return true;
}

dist_monitoring_consumer_task::dist_monitoring_consumer_task(uint8_t priority) :
		scheduler_task("dm_consumer", 3048, priority) {
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

	// Parse received queue data to a single json string
	char sensorBuffer[65];
	char pinBuffer[65];
	char osBuffer[100];
	char taskBuffer[200]; // additional buffer for the top three tasks
	char uart3Buffer[430];
	sprintf(sensorBuffer, "\"temp\": %0.2f, \"light\": %u, \"x\": %i, \"y\": %i, \"z\": %i, ", sensor.temp, sensor.light, sensor.x, sensor.y, sensor.z);
	sprintf(pinBuffer, "\"sw\": [%i, %i,  %i,  %i], ", pin.sw1, pin.sw2, pin.sw3, pin.sw4);
	sprintf(osBuffer, "\"mem\": {\"globalUsed\": %5d, \"mallocUsed\": %5d, \"mallocAvail\": %5d, \"systemAvail\": %5d }, ", os.globalUsed, os.mallocUsed, os.mallocAvail, os.systemAvail);
	sprintf(taskBuffer, "\"task\": [{\"name\": \"%s\", \"percent\": %2u},{\"name\": \"%s\", \"percent\": %2u},{\"name\": \"%s\", \"percent\": %2u}]", os.taskName[0], os.cpuPercent[0], os.taskName[1], os.cpuPercent[1], os.taskName[2], os.cpuPercent[2]);
	sprintf(uart3Buffer, "{%s%s%s%s}", sensorBuffer ,pinBuffer, osBuffer, taskBuffer);

	// Send json string over Uart3 with a baud rate of 9600
	Uart3& u3 = Uart3::getInstance();
	u3.init(9600);
	u3.putline(uart3Buffer);

	// Send debug info
//	printf("Sending json data over Uart3:\n%s\n", uart3Buffer);
//	printf("---------------------------------------------\n");

	// 2 second timeout between transmissions
	vTaskDelay(2000);

	return true;

	// TODO: Support reporting for up to 6 tasks:
//	for (unsigned int i = 0; i < os.count; i++) {
//		printf("taskName: %15s\n", os.taskName[i]);
//		vTaskDelay(1000);
//		printf("cpuPercent: %4lu\n", os.cpuPercent[i]);
//		vTaskDelay(1000);
//	}

}

dist_monitoring_sensor_task::dist_monitoring_sensor_task(uint8_t priority) :
		scheduler_task("dm_sensor", 2048, priority) {
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
	data.z = AS.getZ();
	xQueueSend(getSharedObject(DistMonitoringSensorQueueId), &data, portMAX_DELAY);
	return true;
}

dist_monitoring_pin_task::dist_monitoring_pin_task(uint8_t priority) :
		scheduler_task("dm_pin", 2048, priority) {
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
		scheduler_task("dm_os", 2048, priority) {
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
