/**
 * @file   aq.c
 * @Author 02335 team
 * @date   October, 2024
 * @brief  Alarm queue skeleton implementation
 */

#include "aq.h"
#include <stdlib.h>

typedef struct MsgNode {
    void * data;       // Pointer to the message data
    MsgKind kind;     // Type of the message (alarm or normal)
    struct MsgNode *next; // Pointer to the next message in the queue
} MsgNode;

typedef struct {
    MsgNode *head;
    MsgNode *tail;
    int size;
    int alarm_count;
} AlarmQueueStruct;



AlarmQueue aq_create( ) {
    AlarmQueueStruct *aq = (AlarmQueueStruct *)malloc(sizeof(AlarmQueueStruct));

    if (aq != NULL) {
        aq->head = NULL;
        aq->tail = NULL;
        aq->size = 0;
        aq->alarm_count = 0;
    }

    return aq;

  return NULL;
}

int aq_send(AlarmQueue aq, void *msg, MsgKind k) {
    // Cast the void * to the appropriate struct type
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

    // Check for a null queue or message
    if (!queue || !msg) return AQ_NO_ROOM;

    // Create a new message node
    MsgNode *new_msg = (MsgNode *)malloc(sizeof(MsgNode));
    if (!new_msg) return AQ_NO_ROOM; // Memory allocation failed

    // Initialize the new message
    new_msg->data = msg;
    new_msg->kind = k;
    new_msg->next = NULL;

    // Handle alarm messages
    if (k == AQ_ALARM) {
        // Check if there is already an alarm message
        if (queue->alarm_count > 0) {
            free(new_msg); // Clean up the allocated memory
            return AQ_NO_ROOM;
        }

        // Insert the alarm message at the head of the queue
        new_msg->next = queue->head;
        queue->head = new_msg;

        // If the queue was empty, also set the tail to this message
        if (queue->tail == NULL) {
            queue->tail = new_msg;
        }

        // Update alarm count
        queue->alarm_count = 1;
    } else {
        // Handle normal messages - append to the tail of the queue
        if (queue->tail) {
            queue->tail->next = new_msg;
        } else {
            // If the queue was empty, set the head to the new message
            queue->head = new_msg;
        }
        queue->tail = new_msg;
    }

    // Increment the size of the queue
    queue->size++;
    return 0; // Success
}

int aq_recv(AlarmQueue aq, void **msg) {
    // Cast the void * to the appropriate struct type
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

    // Check for null queue or message pointer
    if (!queue || !msg) return AQ_NO_MSG;

    // Check if the queue is empty
    if (queue->head == NULL) return AQ_NO_MSG;

    // Retrieve the message at the head of the queue
    MsgNode *received_msg = queue->head;
    *msg = received_msg->data; // Assign the output message data

    // Update the head of the queue
    queue->head = received_msg->next;

    // If the queue is now empty, also reset the tail pointer
    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    // Update the alarm count if the message was an alarm
    if (received_msg->kind == AQ_ALARM) {
        queue->alarm_count = 0;
    }

    // Decrement the size of the queue
    queue->size--;

    // Store the message type to return later
    int kind = received_msg->kind;

    // Free the memory for the retrieved message
    free(received_msg);

    // Return the kind of the message
    return kind;
}

int aq_size(AlarmQueue aq) {
    // Cast the void * to the appropriate struct type
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

    // Check for null queue
    if (!queue) return 0;

    // Return the current size of the queue
    return queue->size;
}

int aq_alarms(AlarmQueue aq) {
    // Cast the void * to the appropriate struct type
    AlarmQueueStruct *queue = (AlarmQueueStruct *)aq;

    // Check for null queue
    if (!queue) return 0;

    // Return the number of alarm messages (0 or 1)
    return queue->alarm_count;
}



