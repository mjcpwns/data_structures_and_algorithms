
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Queue.h"

int main(void) {
	Queue q = QueueNew();
	
	for (int num = 0; num < 5000; num++) {
		// enqueue
		for (int i = 1; i <= 500; i++) {
			QueueEnqueue(q, i);
		}

		// dequeue
		for (int j = 1; j <= 250; j++) {
			QueueDequeue(q);
		}

		// enqueue
		for (int i = 1; i <= 500; i++) {
			QueueEnqueue(q, i);
		}

		// dequeue
		for (int j = 1; j <= 750; j++) {
			QueueDequeue(q);
		}
	}

	QueueFree(q);
}

