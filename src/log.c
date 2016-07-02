#include <stdlib.h>
#include <string.h>
#include "log.h"

void
delete_message(struct message *msg)
{
    if (msg->next != NULL)
        delete_message(msg->next);

    free(msg);
}

struct message *
create_message(struct message *log, char *text)
{
    struct message *msg = malloc(sizeof(struct message));

    if (msg != NULL) {
        strcpy(msg->text, text);
        msg->next = log;
    }

    return msg;
}

void
append_message(struct log *log, char *text)
{
    struct message *prev = log->last;
    log->last = create_message(NULL, text);
    prev->next = log->last;
}
