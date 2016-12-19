#ifndef LABS_HPP_
#define LABS_HPP_

#include "scheduler_task.hpp"
#include "shared_handles.h"
#include "FreeRTOS.h"
#include "tasks.hpp"
#include "lpc_sys.h"

// Shared Object ID for shared Queues
typedef enum {
	DistMonitoringSensorQueueId,
	DistMonitoringPinQueueId,
	DistMonitoringOsQueueId,
} sharedHandleId_t;

struct sensorData {
	float temp;
	uint8_t light;
	int x;
	int y;
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

/*
 * Tasks for Final Project - FreeRtos Distributed Monitoring
 */
class dist_monitoring_consumer_task: public scheduler_task {
public:
	dist_monitoring_consumer_task(uint8_t priority);bool init(void);bool run(
			void* p);
};

class dist_monitoring_sensor_task: public scheduler_task {
public:
	dist_monitoring_sensor_task(uint8_t priority);bool init(void);bool run(
			void* p);
};

class dist_monitoring_pin_task: public scheduler_task {
public:
	dist_monitoring_pin_task(uint8_t priority);bool init(void);bool run(
			void* p);
};

class dist_monitoring_os_task: public scheduler_task {
public:
	dist_monitoring_os_task(uint8_t priority);bool init(void);bool run(void* p);
};

/*
 * END Tasks for Final Project
 */
#endif /* LABS_HPP_ */
