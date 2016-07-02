#ifndef __LOG_H__
#define __LOG_H__

#define MAX_MESSAGE_LEN  120

struct message {
    char            text[MAX_MESSAGE_LEN];
    struct message *next;
};

struct log {
    struct message *head;
    struct message *last;
};

struct message *create_message(struct message *, char *);

void append_message(struct log *, char *);

#endif
