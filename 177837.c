int tm_publish(
  
  char       *name,  /* in  */
  void       *info,  /* in  */
  int         len,  /* in  */
  tm_event_t *event)  /* out */

  {
  int rc = TM_SUCCESS;
  struct tcp_chan *chan = NULL;

  if (!init_done)
    return TM_BADINIT;

  *event = new_event();

  if (startcom(TM_POSTINFO, *event, &chan) != DIS_SUCCESS)
    return TM_ESYSTEM;

  if (diswst(chan, name) != DIS_SUCCESS)
    {
    rc = TM_ESYSTEM;
    goto tm_publish_cleanup;
    }

  if (diswcs(chan, (char *)info, len) != DIS_SUCCESS)
    {
    rc = TM_ESYSTEM;
    goto tm_publish_cleanup;
    }

  DIS_tcp_wflush(chan);

  add_event(*event, TM_ERROR_NODE, TM_POSTINFO, NULL);

tm_publish_cleanup:
  if (chan != NULL)
    DIS_tcp_cleanup(chan);

  return rc;
  } /* tm_publish() */