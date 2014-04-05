#ifndef __sleep_h
#define __sleep_h

void sleep(long);
void usleep(long);

void sleep(long millis) {
	int i = 0, j = 0, seconds = 0, milliseconds = 0;
	seconds = millis/1000;
	milliseconds = millis%1000;
	
	for(j = 0; j < seconds; j++) {
		for(i = 0; i < 30000; i++);
	}
	for(i = 0; i < milliseconds; i++) {
		for(j = 0; j < 30; j++);
	}
}

void usleep(long microseconds) {
	int i = 0;
	
	for(i = 0; i < microseconds/33; i++);
}

#endif
