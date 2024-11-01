/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include <stdlib.h>
#include <pthread.h>

typedef struct MsgNode {
    void * data;
    MsgKind kind;
    struct MsgNode *next;
} MsgNode;

typedef struct {
    MsgNode *head;
    MsgNode *tail;
    int size;
    int alarm_count;
    pthread_mutex_t mutex;
    pthread_cond_t aq_alarm_full;
    pthread_cond_t aq_not_empty;
} AlarmQueueStruct;



AlarmQueue aq_create( ) {
    AlarmQueueStruct *aq = (AlarmQueueStruct *)malloc(sizeof(AlarmQueueStruct));

    if (aq != NULL) {
        aq->head = NULL;
        aq->tail = NULL;
        aq->size = 0;
        aq->alarm_count = 0;
        pthread_mutex_init(&aq->mutex, NULL);
        pthread_cond_init(&aq->aq_alarm_full, NULL);
        pthread_cond_init(&aq->aq_not_empty, NULL);
    }

    return aq;

    return NULL;
}

int aq_send(AlarmQueue aq, void *msg, MsgKind k) {

    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

    if (!queue || !msg) return AQ_NO_ROOM;

    MsgNode *new_msg = (MsgNode *)malloc(sizeof(MsgNode));
    if (!new_msg) return AQ_NO_ROOM;

    new_msg->data = msg;
    new_msg->kind = k;
    new_msg->next = NULL;

    pthread_mutex_lock(&queue->mutex);

    while (k==AQ_ALARM && queue->alarm_count > 0) {
        pthread_cond_wait(&queue->aq_alarm_full, &queue->mutex);
    }

    if (k == AQ_ALARM) {
        new_msg->next = queue->head;
        queue->head = new_msg;

        if (queue->tail == NULL) {
            queue->tail = new_msg;
        }
        queue->alarm_count = 1;

    } else {
        if (queue->tail) {
            queue->tail->next = new_msg;
        } else {
            queue->head = new_msg;
        }
        queue->tail = new_msg;
    }

    queue->size++;
    pthread_cond_signal(&queue->aq_not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

int aq_recv(AlarmQueue aq, void **msg) {
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
    if (!queue || !msg) return AQ_NO_MSG;

    pthread_mutex_lock(&queue->mutex);

    if (queue->head == NULL) {
        pthread_cond_wait(&queue->aq_not_empty, &queue->mutex);
    }

    MsgNode *received_msg = queue->head;
    *msg = received_msg->data;

    queue->head = received_msg->next;

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    if (received_msg->kind == AQ_ALARM) {
        queue->alarm_count = 0;
        pthread_cond_signal(&queue->aq_alarm_full);
    }

    queue->size--;

    int kind = received_msg->kind;
    free(received_msg);

    pthread_mutex_unlock(&queue->mutex);

    return kind;
}

int aq_size(AlarmQueue aq) {
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
    if (!queue) return 0;

    pthread_mutex_lock(&queue->mutex);
    int size = queue->size;
    pthread_mutex_unlock(&queue->mutex);

    return size;
}

int aq_alarms(AlarmQueue aq) {
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
    if (!queue) return 0;

    pthread_mutex_lock(&queue->mutex);
    int alarm_count = queue->alarm_count;
    pthread_mutex_unlock(&queue->mutex);

    return alarm_count;
}