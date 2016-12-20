#ifndef MICROWATCH_HPP_
#define MICROWATCH_HPP_

#include "scheduler_task.hpp"
#include "shared_handles.h"
#include "FreeRTOS.h"
#include "tasks.hpp"

// Shared Object ID for shared Queues
typedef enum {
	Lab6QueueId,
	Lab7SensorQueueId,
	DistMonitoringSensorQueueId,
	DistMonitoringPinQueueId,
	DistMonitoringOsQueueId,
} sharedHandleId_t;


/*
 * Tasks for Final Project - FreeRtos Distributed Monitoring
 */

struct sensorData {
	float temp;
	uint8_t light;
	int x;
	int y;
	int z;
};

struct pinData {
	int sw1;
	int sw2;
	int sw3;
	int sw4;
};

struct osData {
	const char* taskName[16];
	uint32_t cpuPercent[16];
	unsigned long count;
	char buffer[512];
	int globalUsed;
	int mallocUsed;
	int mallocAvail;
	int systemAvail;
};

// Parse data from all three queues into a string of json and send over Uart3
class dist_monitoring_consumer_task: public scheduler_task {
public:
	dist_monitoring_consumer_task(uint8_t priority);bool init(void);bool run(
			void* p);
};
// Clear data from all three queues to maintain freshness
// NOTE: this task may slightly increase the time needed for the consumer task to parse data
// WARNING: This task should ALWAYS run at a lower priority level than the consumer task
class dist_monitoring_data_fresh_task: public scheduler_task {
public:
	dist_monitoring_data_fresh_task(uint8_t priority);bool init(void);bool run(
			void* p);
};

// Collect sensor data in sensorData object and send to queue
class dist_monitoring_sensor_task: public scheduler_task {
public:
	dist_monitoring_sensor_task(uint8_t priority);bool init(void);bool run(
			void* p);
};

// Collect pin data in pinData object and send to queue
class dist_monitoring_pin_task: public scheduler_task {
public:
	dist_monitoring_pin_task(uint8_t priority);bool init(void);bool run(
			void* p);
};

// Collect OS data in osData object and send to queue
class dist_monitoring_os_task: public scheduler_task {
public:
	dist_monitoring_os_task(uint8_t priority);bool init(void);bool run(void* p);
};

/*
 * END Tasks for Final Project
 */
#endif /* MICROWATCH_HPP_ */
