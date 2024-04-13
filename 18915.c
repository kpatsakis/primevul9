setup_bus_child (gpointer data)
{
  A11yBusLauncher *app = data;
  (void) app;

  close (app->pipefd[0]);
  dup2 (app->pipefd[1], 3);
  close (app->pipefd[1]);

  /* On Linux, tell the bus process to exit if this process goes away */
#ifdef __linux
#include <sys/prctl.h>
  prctl (PR_SET_PDEATHSIG, 15);
#endif  
}