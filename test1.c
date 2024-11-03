#include "aq.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *send_alarm(void *arg) {
    AlarmQueue *q = (AlarmQueue *)arg;
    int msg = 1;
    printf("Sending alarm message...\n");
    aq_send(q, &msg, AQ_ALARM);
    printf("Alarm message sent.\n");
    return NULL;
}

void *receive_alarm(void *arg) {
    AlarmQueue *q = (AlarmQueue *)arg;
    void *msg;
    sleep(1); // Ensure this runs after the first send
    printf("Receiving alarm message...\n");
    aq_recv(q, &msg);
    printf("Alarm message received.\n");
    return NULL;
}

int main() {
    AlarmQueue *q = aq_create();
    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, send_alarm, q);
    pthread_create(&t2, NULL, send_alarm, q);
    pthread_create(&t3, NULL, receive_alarm, q);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}