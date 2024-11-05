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

void *send_normal(void *arg) {
    AlarmQueue *q = (AlarmQueue *)arg;
    int msg = 1;
    printf("Sending normal message...\n");
    aq_send(q, &msg, AQ_NORMAL);
    printf("Normal message sent.\n");
    return NULL;
}

void *receive_message(void *arg) {
    AlarmQueue *q = (AlarmQueue *)arg;
    void *msg;
    for (int i = 0; i < 3; i++) {
        sleep(3);
        printf("Receiving message...\n");
        int kind = aq_recv(q, &msg);
        if (kind == AQ_ALARM) {
            printf("Alarm message received.\n");
        } else if (kind == AQ_NORMAL) {
            printf("Normal message received.\n");
        } else {
            printf("No message received.\n");
        }
    }
    return NULL;
}

int main() {
    AlarmQueue *q = aq_create();
    pthread_t t1, t2, t3, t4;

    pthread_create(&t1, NULL, send_alarm, q);
    pthread_create(&t2, NULL, send_alarm, q);
    pthread_create(&t3, NULL, send_normal, q);
    pthread_create(&t4, NULL, receive_message, q);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    return 0;
}