int tm_subscribe(
  
  tm_task_id  tid,     /* in  */
  char       *name,    /* in  */
  void       *info,    /* out */
  int         len,     /* in  */
  int        *info_len,/* out */
  tm_event_t *event)    /* out */
  
  {
  int              rc = TM_SUCCESS;
  task_info       *tp;
  struct tcp_chan *chan = NULL;

  struct infohold *ihold;

  if (!init_done)
    {
    rc = TM_BADINIT;
    goto tm_subscribe_cleanup;
    }

  if ((tp = find_task(tid)) == NULL)
    {
    rc = TM_ENOTFOUND;
    goto tm_subscribe_cleanup;
    }

  *event = new_event();

  if (startcom(TM_GETINFO, *event, &chan) != DIS_SUCCESS)
    {
    rc = TM_ESYSTEM;
    goto tm_subscribe_cleanup;
    }

  if (diswsi(chan, tp->t_node) != DIS_SUCCESS)
    {
    rc = TM_ESYSTEM;
    goto tm_subscribe_cleanup;
    }

  if (diswsi(chan, tid) != DIS_SUCCESS)
    {
    rc = TM_ESYSTEM;
    goto tm_subscribe_cleanup;
    }

  if (diswst(chan, name) != DIS_SUCCESS)
    {
    rc = TM_ESYSTEM;
    goto tm_subscribe_cleanup;
    }

  DIS_tcp_wflush(chan);

  ihold = (struct infohold *)calloc(1, sizeof(struct infohold));

  assert(ihold != NULL);

  ihold->info = info;

  ihold->len = len;

  ihold->info_len = info_len;

  add_event(*event, tp->t_node, TM_GETINFO, (void *)ihold);

tm_subscribe_cleanup:
  if (chan != NULL)
    DIS_tcp_cleanup(chan);

  return rc;
  }