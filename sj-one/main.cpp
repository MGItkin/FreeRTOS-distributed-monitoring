// main.cpp code to launch MicroWatch tasks

scheduler_add_task(new dist_monitoring_consumer_task(PRIORITY_HIGH));
scheduler_add_task(new dist_monitoring_data_fresh_task(PRIORITY_MEDIUM));
scheduler_add_task(new dist_monitoring_sensor_task(PRIORITY_LOW));
scheduler_add_task(new dist_monitoring_pin_task(PRIORITY_LOW));
scheduler_add_task(new dist_monitoring_os_task(PRIORITY_LOW));