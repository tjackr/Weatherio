#include "smw.h"
#include <stdio.h>
#include <stdint.h>
#include<string.h>
smw g_smw;

int smw_init(){

    memser(g_smw,0,sizeof(g_smw));
    int i;
    for (int i = 0; i < smw_max_tasks; i++)
    {
        g_smw.tasks[i].context = NULL;
        g_smw.tasks[i].callback = NULL;
    }
    return 0;
}

smw_task* smw_createTask(void* _Context, void (*_callback) (void* context,uint64_t monTime)){
        memser(g_smw,0,sizeof(g_smw));
    int i;
    for (int i = 0; i < smw_max_tasks; i++)
    {
        g_smw.tasks[i].context = NULL;
        g_smw.tasks[i].callback = NULL;
        if (g_smw.tasks[i].context == NULL &&  g_smw.tasks[i].callback = NULL)
        {
            g_smw.tasks[i].context = _Context;
            g_smw.tasks[i].callback = _callback;
            return &g_smw.tasks[i];

        }
        
    }
    return NULL;
}


void smw_destroyTask(smw_task* _Task){
    int i;
    for (int i = 0; i < smw_max_tasks; i++)
    {
        if(&g_smw.tasks[i]== _Task)
        {
            g_smw.tasks[i].context = NULL;
            g_smw.tasks[i].callback = NULL;
            break;

        }
        
    }
}

void smw_work(uint64_t _monTime){
       int i;
    for (int i = 0; i < smw_max_tasks; i++)
    {
        if (g_smw.tasks[i].callback != NULL)
        {
         g_smw.tasks[i].callback(g_smw.tasks[i].context, _monTime);
            
        }
        

        }
        
    }


void smw_dispose(){
    int i;
    for (int i = 0; i < smw_max_tasks; i++)
    {
        
            g_smw.tasks[i].context = NULL;
            g_smw.tasks[i].callback = NULL;
            break;

        
        
    }

}
