#include "aq.h"
#include <pthread.h>
#include <stdio.h>

typedef struct {
    AlarmQueue *q;
    int msg;
} ThreadArgs;

void *send_normal(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    printf("Sending normal message %d...\n", args->msg);
    int result = aq_send(args->q, &(args->msg), AQ_NORMAL);
    if (result != 0) {
        printf("Failed to send normal message %d\n", args->msg);
    } else {
        printf("Normal message %d sent.\n", args->msg);
    }
    return NULL;
}

void *receive_messages(void *arg) {
    AlarmQueue *q = (AlarmQueue *)arg;
    void *msg;
    for (int i = 0; i < 3; i++) {
        int result = aq_recv(q, &msg);
        if (result == AQ_NO_MSG) {
            printf("No message received\n");
        } else {
            printf("Received message: %d\n", *(int *)msg);
        }
    }
    return NULL;
}
int main() {
    AlarmQueue q = aq_create();
    if (q == NULL) {
        printf("Failed to create alarm queue\n");
        return 1;
    }

    pthread_t t1, t2, t3, t4;

    ThreadArgs args1 = {q, 1};
    ThreadArgs args2 = {q, 2};
    ThreadArgs args3 = {q, 3};

    pthread_create(&t1, NULL, send_normal, &args1);
    pthread_create(&t2, NULL, send_normal, &args2);
    pthread_create(&t3, NULL, send_normal, &args3);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    pthread_create(&t4, NULL, receive_messages, q);

    pthread_join(t4, NULL);

    return 0;
}
