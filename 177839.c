int tm_poll(

  tm_event_t  poll_event,
  tm_event_t *result_event,
  int         wait,
  int        *tm_errno)

  {
  int  num, i;
  int  ret, mtype, nnodes;
  int  prot, protver;
  int  *obitvalp;
  event_info *ep = NULL;
  tm_task_id tid, *tidp;
  tm_event_t nevent;
  tm_node_id node;
  char  *jobid = NULL;
  char  *info = NULL;

  struct tm_roots *roots;

  struct taskhold *thold;

  struct infohold *ihold;

  struct reschold *rhold;
  extern time_t pbs_tcp_timeout;

  if (!init_done)
    {
    return(TM_BADINIT);
    }

  if (result_event == NULL)
    return(TM_EBADENVIRONMENT);

  *result_event = TM_ERROR_EVENT;

  if (poll_event != TM_NULL_EVENT)
    return(TM_ENOTIMPLEMENTED);

  if (tm_errno == NULL)
    return(TM_EBADENVIRONMENT);

  if (event_count == 0)
    {
    TM_DBPRT(("%s: no events waiting\n",
           __func__))

    return(TM_ENOTFOUND);
    }

  if (local_conn < 0)
    {
    TM_DBPRT(("%s: INTERNAL ERROR %d events but no connection (%d)\n",
           __func__, event_count, local_conn))

    if (static_chan != NULL)
      {
      DIS_tcp_cleanup(static_chan);
      static_chan = NULL;
      }
    return(TM_ENOTCONNECTED);
    }

  if ((static_chan == NULL) && ((static_chan = DIS_tcp_setup(local_conn)) == NULL))
    {
    TM_DBPRT(("%s: Error allocating memory for sock buffer %d", __func__, PBSE_MEM_MALLOC))
      return TM_BADINIT;
    }

  /*
  ** Setup tcp dis routines with a wait value appropriate for
  ** the value of wait the user set.
  */
  pbs_tcp_timeout = wait ? FOREVER : 1;

  prot = disrsi(static_chan, &ret);

  if (ret == DIS_EOD)
    {
    *result_event = TM_NULL_EVENT;
    DIS_tcp_cleanup(static_chan);
    static_chan = NULL;
    return TM_SUCCESS;
    }
  else if (ret != DIS_SUCCESS)
    {
    TM_DBPRT(("%s: protocol number dis error %d\n", __func__, ret))
    goto tm_poll_error;
    }

  if (prot != TM_PROTOCOL)
    {
    TM_DBPRT(("%s: bad protocol number %d\n", __func__, prot))
    goto tm_poll_error;
    }

  /*
  ** We have seen the start of a message.  Set the timeout value
  ** so we wait for the remaining data of a message.
  */
  pbs_tcp_timeout = FOREVER;

  protver = disrsi(static_chan, &ret);

  if (ret != DIS_SUCCESS)
    {
    TM_DBPRT(("%s: protocol version dis error %d\n", __func__, ret))
    goto tm_poll_error;
    }

  if (protver != TM_PROTOCOL_VER)
    {
    TM_DBPRT(("%s: bad protocol version %d\n", __func__, protver))
    goto tm_poll_error;
    }

  mtype = disrsi(static_chan, &ret);

  if (ret != DIS_SUCCESS)
    {
    TM_DBPRT(("%s: mtype dis error %d\n", __func__, ret))
    goto tm_poll_error;
    }

  nevent = disrsi(static_chan, &ret);

  if (ret != DIS_SUCCESS)
    {
    TM_DBPRT(("%s: event dis error %d\n", __func__, ret))
    goto tm_poll_error;
    }

  *result_event = nevent;

  TM_DBPRT(("%s: got event %d return %d\n", __func__, nevent, mtype))

  if ((ep = find_event(nevent)) == NULL)
    {
    TM_DBPRT(("%s: No event found for number %d\n", __func__, nevent));
    DIS_tcp_close(static_chan);
    static_chan = NULL;
    local_conn = -1;
    return TM_ENOEVENT;
    }

  if (mtype == TM_ERROR)   /* problem, read error num */
    {
    *tm_errno = disrsi(static_chan, &ret);
    TM_DBPRT(("%s: event %d error %d\n", __func__, nevent, *tm_errno));
    goto tm_poll_done;
    }

  *tm_errno = TM_SUCCESS;

  switch (ep->e_mtype)
    {

      /*
      ** auxiliary info (
      **  number of nodes int;
      **  nodeid[0] int;
      **  ...
      **  nodeid[n-1] int;
      **  parent jobid string;
      **  parent nodeid int;
      **  parent taskid int;
      ** )
      */

    case TM_INIT:
      nnodes = disrsi(static_chan, &ret);

      if (ret != DIS_SUCCESS)
        {
        TM_DBPRT(("%s: INIT failed nnodes\n", __func__))
        goto tm_poll_error;
        }

      node_table = (tm_node_id *)calloc(nnodes + 1,

                                        sizeof(tm_node_id));

      if (node_table == NULL)
        {
        perror("Memory allocation failed");
        goto tm_poll_error;
        }

      TM_DBPRT(("%s: INIT nodes %d\n", __func__, nnodes))

      for (i = 0; i < nnodes; i++)
        {
        node_table[i] = disrsi(static_chan, &ret);

        if (ret != DIS_SUCCESS)
          {
          TM_DBPRT(("%s: INIT failed nodeid %d\n", __func__, i))
          goto tm_poll_error;
          }
        }

      node_table[nnodes] = TM_ERROR_NODE;

      jobid = disrst(static_chan, &ret);

      if (ret != DIS_SUCCESS)
        {
        TM_DBPRT(("%s: INIT failed jobid\n", __func__))
        goto tm_poll_error;
        }

      TM_DBPRT(("%s: INIT daddy jobid %s\n", __func__, jobid))

      node = disrsi(static_chan, &ret);

      if (ret != DIS_SUCCESS)
        {
        TM_DBPRT(("%s: INIT failed parent nodeid\n", __func__))
        goto tm_poll_error;
        }

      TM_DBPRT(("%s: INIT daddy node %d\n", __func__, node))

      tid = disrsi(static_chan, &ret);

      if (ret != DIS_SUCCESS)
        {
        TM_DBPRT(("%s: INIT failed parent taskid\n", __func__))
        goto tm_poll_error;
        }

      TM_DBPRT(("%s: INIT daddy tid %lu\n", __func__, (unsigned long)tid))

      roots = (struct tm_roots *)ep->e_info;
      roots->tm_parent = new_task(jobid, node, tid);
      roots->tm_me = new_task(tm_jobid,
                              tm_jobndid,
                              tm_jobtid);
      roots->tm_nnodes = nnodes;
      roots->tm_ntasks = 0;  /* TODO */
      roots->tm_taskpoolid = -1; /* what? */
      roots->tm_tasklist = NULL; /* TODO */

      break;

    case TM_TASKS:
      thold = (struct taskhold *)ep->e_info;
      tidp = thold->list;
      num = thold->size;

      for (i = 0;; i++)
        {
        tid = disrsi(static_chan, &ret);

        if (tid == TM_NULL_TASK)
          break;

        if (ret != DIS_SUCCESS)
          goto tm_poll_error;

        if (i < num)
          {
          tidp[i] = new_task(tm_jobid,
                             ep->e_node, tid);
          }
        }

      if (i < num)
        tidp[i] = TM_NULL_TASK;

      *(thold->ntasks) = i;

      break;

    case TM_SPAWN:
      tid = disrsi(static_chan, &ret);

      if (ret != DIS_SUCCESS)
        {
        TM_DBPRT(("%s: SPAWN failed tid\n", __func__))
        goto tm_poll_error;
        }

      tidp = (tm_task_id *)ep->e_info;

      *tidp = new_task(tm_jobid, ep->e_node, tid);
      break;

    case TM_SIGNAL:
      break;

    case TM_OBIT:
      obitvalp = (int *)ep->e_info;
      *obitvalp = disrsi(static_chan, &ret);

      if (ret != DIS_SUCCESS)
        {
        TM_DBPRT(("%s: OBIT failed obitval\n", __func__))
        goto tm_poll_error;
        }

      break;

    case TM_POSTINFO:
      break;

    case TM_GETINFO:
      ihold = (struct infohold *)ep->e_info;
      info = disrcs(static_chan, (size_t *)ihold->info_len, &ret);

      if (ret != DIS_SUCCESS)
        {
        if (info != NULL)
          free(info);

        TM_DBPRT(("%s: GETINFO failed info\n", __func__))
        break;
        }

      memcpy(ihold->info, info, MIN(*ihold->info_len, ihold->len));

      free(info);
      break;

    case TM_RESOURCES:
      rhold = (struct reschold *)ep->e_info;
      info = disrst(static_chan, &ret);

      if (ret != DIS_SUCCESS)
        {
        if (info != NULL)
          free(info);

        break;
        }

      snprintf(rhold->resc, rhold->len, "%s", info);

      free(info);

      break;

    default:
      TM_DBPRT(("%s: unknown event command %d\n", __func__, ep->e_mtype))
      goto tm_poll_error;
    }

  DIS_tcp_wflush(static_chan);
tm_poll_done:

  if (jobid != NULL)
    free(jobid);

  del_event(ep);
  if (tcp_chan_has_data(static_chan) == FALSE)
    {
    DIS_tcp_cleanup(static_chan);
    static_chan = NULL;
    }

  return TM_SUCCESS;

tm_poll_error:

  if (jobid != NULL)
    free(jobid);

  if (ep)
    del_event(ep);

  close(local_conn);
  DIS_tcp_cleanup(static_chan);
  static_chan = NULL;
  local_conn = -1;

  return TM_ENOTCONNECTED;
  }