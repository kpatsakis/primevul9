int tm_taskinfo(

  tm_node_id  node,  /* in  */
  tm_task_id *tid_list, /* out */
  int         list_size, /* in  */
  int        *ntasks, /* out */
  tm_event_t *event)  /* out */

  {
  struct taskhold *thold;
  struct tcp_chan *chan = NULL;

  if (!init_done)
    return TM_BADINIT;

  if (tid_list == NULL || list_size == 0 || ntasks == NULL)
    return TM_EBADENVIRONMENT;

  *event = new_event();

  if (startcom(TM_TASKS, *event, &chan) != DIS_SUCCESS)
    return TM_ESYSTEM;

  if (diswsi(chan, node) != DIS_SUCCESS)
    {
    DIS_tcp_cleanup(chan);
    return TM_ESYSTEM;
    }

  DIS_tcp_wflush(chan);
  DIS_tcp_cleanup(chan);

  thold = (struct taskhold *)calloc(1, sizeof(struct taskhold));

  assert(thold != NULL);

  thold->list = tid_list;

  thold->size = list_size;

  thold->ntasks = ntasks;

  add_event(*event, node, TM_TASKS, (void *)thold);

  return TM_SUCCESS;
  }