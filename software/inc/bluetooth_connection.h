#ifndef BLUETOOTH_CONNECTION_H
#define BLUETOOTH_CONNECTION_H

#define HEARTBEAT_PERIOD_MS 1000

void bluetooth_init();

int bluetooth_send_telemetry(const char * data);


#endif 