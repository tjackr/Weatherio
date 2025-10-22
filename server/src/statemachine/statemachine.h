#ifndef _smw.h_
#define _smw.h_
#include "../libs/utils/linkedlist.h"

#ifndef smw_max_tasks
#define smw_max_tasks 16
#endif

typedef struct {
    void* context;
    void (*callback)(void* context, uint64_t monTime);

}smw_task;


typedef struct 
{
    smw_task tasks[smw_max_tasks];
}smw;

extern smw g_smw;

int smw_init();
int smw_createTask(void* _Context, void (*_callback) (void* context,uint64_t monTime));
void smw_destroyTask(smw_task* _Task);

void smw_work(uint64_t _monTime);
void smw_dispose();
#endif




