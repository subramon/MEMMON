In your main program you will need

#include "rs_malloc.h" // This redfines malloc and free
mmon_t g_mmon;  
// Redfine free and malloc as follows
#define malloc(x) { rs_malloc(x, __FILE__, __LINE__, __FUNCTION__, "") }
#define free(x)   { rs_free(x); }
// TODO P1 Note sure we need above

First thing you need to do is to initialize g_mmon
  status = init_mmon(&g_mmon); cBYE(status);

To make sure that everythng that was malloc'd has been freed
  status = chck_mmon(&g_mmon); cBYE(status);

To print the current status of malloc/free
  status = prnt_mmon(&g_mmon); cBYE(status);

Last thing to do is to release g_mmon
  status = free_mmon(&g_mmon); cBYE(status);
