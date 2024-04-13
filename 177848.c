int tm_obit(

  tm_task_id  tid,  /* in  */
  int        *obitval, /* out */
  tm_event_t *event)  /* out */

  {
  int rc = TM_SUCCESS;
  task_info *tp;
  struct tcp_chan *chan = NULL;

  if (!init_done)
    {
    rc = TM_BADINIT;
    goto tm_obit_cleanup;
    }

  if ((tp = find_task(tid)) == NULL)
    {
    rc = TM_ENOTFOUND;
    goto tm_obit_cleanup;
    }

  *event = new_event();

  if (startcom(TM_OBIT, *event, &chan) != DIS_SUCCESS)
    {
    rc = TM_ESYSTEM;
    goto tm_obit_cleanup;
    }

  if (diswsi(chan, tp->t_node) != DIS_SUCCESS)
    {
    rc = TM_ESYSTEM;
    goto tm_obit_cleanup;
    }

  if (diswsi(chan, tid) != DIS_SUCCESS)
    {
    rc = TM_ESYSTEM;
    goto tm_obit_cleanup;
    }

  DIS_tcp_wflush(chan);

  add_event(*event, tp->t_node, TM_OBIT, (void *)obitval);

tm_obit_cleanup:
  if (chan != NULL)
    DIS_tcp_cleanup(chan);
  return rc;
  }