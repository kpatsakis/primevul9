int tm_init(

  void             *info,  /* in, currently unused */
  struct  tm_roots *roots) /* out */

  {
  tm_event_t  nevent, revent;
  char   *env, *hold;
  int   err;
  int   nerr = 0;
  struct tcp_chan *chan = NULL;

  if (init_done)
    {
    return(TM_BADINIT);
    }

  if ((tm_jobid = getenv("PBS_JOBID")) == NULL)
    {
    return(TM_EBADENVIRONMENT);
    }

  tm_jobid_len = strlen(tm_jobid);

  if ((tm_jobcookie = getenv("PBS_JOBCOOKIE")) == NULL)
    return TM_EBADENVIRONMENT;

  tm_jobcookie_len = strlen(tm_jobcookie);

  if ((env = getenv("PBS_NODENUM")) == NULL)
    return TM_EBADENVIRONMENT;

  tm_jobndid = (tm_node_id)strtol(env, &hold, 10);

  if (env == hold)
    return TM_EBADENVIRONMENT;

  if ((env = getenv("PBS_TASKNUM")) == NULL)
    return TM_EBADENVIRONMENT;

  if ((tm_jobtid = atoi(env)) == 0)
    return TM_EBADENVIRONMENT;

  if ((env = getenv("PBS_MOMPORT")) == NULL)
    return TM_EBADENVIRONMENT;

  if ((tm_momport = atoi(env)) == 0)
    return TM_EBADENVIRONMENT;

  init_done = 1;

  nevent = new_event();

  /*
   * send the following request:
   * header  (tm_init)
   * int  node number
   * int  task number
   */

  if (startcom(TM_INIT, nevent, &chan) != DIS_SUCCESS)
    return TM_ESYSTEM;

  DIS_tcp_wflush(chan);
  DIS_tcp_cleanup(chan);

  add_event(nevent, TM_ERROR_NODE, TM_INIT, (void *)roots);

  while (TRUE)
    {
    if ((err = tm_poll(TM_NULL_EVENT, &revent, 1, &nerr)) != TM_SUCCESS)
      return err;
    if (event_count == 0)
      break;
    }

  return nerr;
  }