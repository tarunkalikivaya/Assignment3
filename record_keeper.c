#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <math.h>
//#include "db.h"
#include "msgq.h"
#define SID 5678
#define CID 8765

data *head = NULL;
data *end = NULL;
static int count = 0;

int getLength(char *s)
{
    int length = 0;
    for (int i = 0; s[i] != '\0'; ++i)
    {
        ++length;
    }
    return length;
}

void Insert(char *name, char *department, int emp_num, int salary)
{
    data *curr = head;
    while (curr != NULL)
    {
        if (curr->emp_num == emp_num)
            return;
        curr = curr->next;
    }
    free(curr);

    data *new = malloc(sizeof(data));
    if (new == NULL)
    {
        printf("malloc failed\n");
        exit(-1);
    }

    if (getLength(name) < 13)
        new->name = name;
    else
        new->name = NULL;

    if (getLength(department) < 13)
        new->department = department;
    else
        new->department = NULL;

    if ((floor(log10(abs(emp_num))) + 1) < 13)
        new->emp_num = emp_num;
    else
        new->emp_num = 0;

    if ((floor(log10(abs(emp_num))) + 1) < 13)
        new->salary = salary;
    else
        new->salary = 0;

    new->next = NULL;
    ++count;

    if (head == NULL)
    {
        head = new;
        end = new;
        return;
    }

    end->next = new;
    end = new;
}

char *Check_name(int emp_num)
{
    data *curr = head;
    while (curr != NULL)
    {
        if (curr->emp_num == emp_num)
            return curr->name;
    }
    free(curr);
    return NULL;
}

char *Check_department(int emp_num)
{
    data *curr = head;
    while (curr != NULL)
    {
        if (curr->emp_num == emp_num)
            return curr->department;
    }
    free(curr);
    return NULL;
}

int Check_salary(int emp_num)
{
    data *curr = head;
    while (curr != NULL)
    {
        if (curr->emp_num == emp_num)
            return curr->salary;
    }
    free(curr);
    return -1;
}

int *Check(char *department)
{
    data *curr = head;
    printf("%d\n", count);
    int *tmp;
    tmp = malloc(count * sizeof(int));
    //tmp = calloc(count, sizeof(int));
    printf("%d\n", count);
    //printf("%d", tmp);
    printf("sizeof of tmp: %lu\n", sizeof(tmp));
    //int *new = NULL;
    int i = 0;
    while (curr != NULL)
    {
        if (strcmp(curr->department, department) == 0)
        {
            tmp[i] = curr->emp_num;
            i++;
        }
        curr = curr->next;
    }
    printf("%d\n", i);
    //tmp = realloc(tmp, i * sizeof(int));
    //printf("%lu\n", sizeof(tmp) / sizeof(tmp[0]));
    return tmp;
}

int Delete(int emp_num)
{

    data *curr = head;
    data *prev = NULL;

    //if list is empty
    if (head == NULL)
        return -1;

    // navigate through list
    while (curr->emp_num != emp_num)
    {
        //if it is last node
        if (curr->next == NULL)
        {
            return -1;
        }
        else
        {
            //store reference to current link
            prev = curr;
            //move to next link
            curr = curr->next;
        }
    }

    --count;
    //found a match, update the link
    if (curr == head)
    {
        //change first to point to next link
        head = head->next;
    }
    else
    {
        //bypass the current link
        prev->next = curr->next;
        //return -1;
    }
    return 0;
}

void print_list(void)
{
    data *curr = head;
    while (curr != NULL)
    {
        printf("%s  %s  %d  %f\n", curr->name, curr->department, curr->emp_num, curr->salary);
        curr = curr->next;
    }
    free(curr);
}

int main(void)
{
    //data *head = NULL;
    message *msgin = malloc(sizeof(message));
    inout *msgout = malloc(sizeof(inout));
    int running = 1;

    // Insert("Joe", "A", 935, 1231);
    // Insert("Abby", "A", 345, 8768);
    // Insert("Dan", "A", 867, 4569);
    // Insert("Joeeeeyyyyy", "A", 455, 1222531);

    // print_list();
    // printf("%d\n", count);
    // printf("\n");
    // //printf("%d\n", Delete(3451));
    // print_list();
    // printf("%d\n", count);

    // int *checked = Check("A");
    // //int len = 12;
    // //int len = sizeof(checked) / sizeof(checked[0]);
    // for (int j = 0; j < 5; j++)
    // {
    //     printf("%d ", checked[j]);
    // }
    // printf("\n");
    // free(checked);

    int msgid_cli = msgget((key_t)CID, 0666 | IPC_CREAT);
    if (msgid_cli == -1)
    {
        fprintf(stderr, "msgget_snd failed with error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int msgid_ser = msgget((key_t)SID, 0666 | IPC_CREAT);
    if (msgid_ser == -1)
    {
        fprintf(stderr, "msgget_rec failed with error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (running)
    {
        char temp[12];

        if (msgrcv(msgid_ser, msgin, sizeof(data), msgin->msg_type, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }

        printf("%ld", msgin->msg_type);

        switch (msgin->msg_type)
        {
        case 1:
            printf("MADE ITTTTTTT");
            Insert(msgin->msg.name, msgin->msg.department, msgin->msg.emp_num, msgin->msg.salary);
            break;
        case 2:
            for (int i = 0; i < getLength(Check_name(msgin->msg.emp_num)); i++)
            {
                msgout->s[i] = Check_name(msgin->msg.emp_num)[i];
            }
            break;
        case 3:
            for (int i = 0; i < getLength(Check_department(msgin->msg.emp_num)); i++)
            {
                msgout->s[i] = Check_department(msgin->msg.emp_num)[i];
            }
            break;
        case 4:
            sprintf(temp, "%f", (double)Check_salary(msgin->msg.emp_num));
            for (int i = 0; i < getLength(temp); i++)
            {
                msgout->s[i] = temp[i];
            }
            break;
        case 5:
            //int *tmp = Check(msgin->msg.department);
            break;
        case 6:
            Delete(msgin->msg.emp_num);
            break;
        default:
            break;
        }

        if (msgsnd(msgid_cli, msgout, sizeof(message), 0) == -1)
        {
            perror("msgsnd");
            exit(1);
        }
    }

    if (msgctl(msgid_cli, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    if (msgctl(msgid_ser, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "msgctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}