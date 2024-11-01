/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include <stdlib.h>

typedef struct MsgNode {
    void *data;
    MsgKind kind;
    struct MsgNode *next;
} MsgNode;

typedef struct {
    MsgNode *head;
    MsgNode *tail;
    int size;
    int alarm_count;
} AlarmQueueStruct;

AlarmQueue aq_create() {
    AlarmQueueStruct *aq = (AlarmQueueStruct *)malloc(sizeof(AlarmQueueStruct));
    if (aq != NULL) {
        aq->head = NULL;
        aq->tail = NULL;
        aq->size = 0;
        aq->alarm_count = 0;
    }
    return aq;
}

int aq_send(AlarmQueue aq, void *msg, MsgKind k) {
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
    if (!queue || !msg) return AQ_NO_ROOM;

    MsgNode *new_msg = (MsgNode *)malloc(sizeof(MsgNode));
    if (!new_msg) return AQ_NO_ROOM;

    new_msg->data = msg;
    new_msg->kind = k;
    new_msg->next = NULL;

    if (k == AQ_ALARM) {
        if (queue->alarm_count > 0) {
            free(new_msg);
            return AQ_NO_ROOM;
        }
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
    return 0;
}

int aq_recv(AlarmQueue aq, void **msg) {
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
    if (!queue || !msg) return AQ_NO_MSG;
    if (queue->head == NULL) return AQ_NO_MSG;

    MsgNode *received_msg = queue->head;
    *msg = received_msg->data;
    queue->head = received_msg->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    if (received_msg->kind == AQ_ALARM) {
        queue->alarm_count = 0;
    }
    queue->size--;
    int kind = received_msg->kind;
    free(received_msg);
    return kind;
}

int aq_size(AlarmQueue aq) {
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
    if (!queue) return 0;
    return queue->size;
}

int aq_alarms(AlarmQueue aq) {
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;
    if (!queue) return 0;
    return queue->alarm_count;
}