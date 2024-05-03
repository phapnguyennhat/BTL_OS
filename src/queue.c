#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t *q)
{
        if (q == NULL)
                return 1;
        return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
        /* TODO: put a new process to queue [q] */
        // if (q->size == MAX_QUEUE_SIZE) // is full
        //         return;
        // q->proc[q->size] = proc;
        // q->size++; // them mot proc co cung do uu tien vao trong queue

        int proc_queue_is_having = q->size;
        if (proc_queue_is_having < MAX_QUEUE_SIZE)
        {
                q->proc[proc_queue_is_having] = proc;
                q->size = proc_queue_is_having + 1;
        }
}

struct pcb_t *dequeue(struct queue_t *q)
{
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */
        int proc_queue_is_having = q->size;
        if (proc_queue_is_having <= 0)
        {
                return NULL;
        }
        else
        {
                struct pcb_t *temp = q->proc[0];
                uint32_t flag_prio = temp->priority;
                int pos = 0;
                for (int i = 0; i < proc_queue_is_having; i++)
                {
                        uint32_t this_prio = q->proc[i]->priority;
                        if (flag_prio < this_prio)
                        {
                                flag_prio = this_prio;
                                pos = i;
                        }
                }
                temp = q->proc[pos];
                for (int i = pos; i < proc_queue_is_having - 1; i++)
                {
                        if (q->proc[i] && q->proc[i + 1])
                        {
                                q->proc[i] = q->proc[i + 1];
                        }
                }
                q->proc[proc_queue_is_having - 1] = NULL;
                q->size--;
                return temp;
        }

        return NULL;
}
