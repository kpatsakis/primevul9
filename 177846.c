static int startcom(

  int               com,
  tm_event_t        event,
  struct tcp_chan **pchan)

  {
  int     ret = DIS_SUCCESS;
  struct tcp_chan *chan = NULL;

  if (localmom() == -1)
    {
    return(-1);
    }

  if ((chan = DIS_tcp_setup(local_conn)) == NULL)
    goto done;

  ret = diswsi(chan, TM_PROTOCOL);

  if (ret != DIS_SUCCESS)
    goto done;

  ret = diswsi(chan, TM_PROTOCOL_VER);

  if (ret != DIS_SUCCESS)
    goto done;

  ret = diswcs(chan, tm_jobid, tm_jobid_len);

  if (ret != DIS_SUCCESS)
    goto done;

  ret = diswcs(chan, tm_jobcookie, tm_jobcookie_len);

  if (ret != DIS_SUCCESS)
    goto done;

  ret = diswsi(chan, com);

  if (ret != DIS_SUCCESS)
    goto done;

  ret = diswsi(chan, event);

  if (ret != DIS_SUCCESS)
    goto done;

  ret = diswui(chan, tm_jobtid);

  if (ret != DIS_SUCCESS)
    goto done;

  *pchan = chan;
  return(DIS_SUCCESS);

done:

  TM_DBPRT(("startcom: send error %s\n",
         dis_emsg[ret]))
  if (chan != NULL)
    DIS_tcp_close(chan);
  else
    close(local_conn);

  local_conn = -1;

  return(ret);
  }  /* END startcom() */