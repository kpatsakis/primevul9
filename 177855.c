int tm_rescinfo(

  tm_node_id  node,  /* in  */
  char       *resource, /* out */
  int         len,  /* in  */
  tm_event_t *event)  /* out */

  {
  struct reschold *rhold;
  struct tcp_chan *chan = NULL;

  if (!init_done)
    return TM_BADINIT;

  if (resource == NULL || len == 0)
    return TM_EBADENVIRONMENT;

  *event = new_event();

  if (startcom(TM_RESOURCES, *event, &chan) != DIS_SUCCESS)
    return TM_ESYSTEM;

  if (diswsi(chan, node) != DIS_SUCCESS)
    {
    DIS_tcp_cleanup(chan);
    return TM_ESYSTEM;
    }

  DIS_tcp_wflush(chan);
  DIS_tcp_cleanup(chan);

  rhold = (struct reschold *)calloc(1, sizeof(struct reschold));

  assert(rhold != NULL);

  rhold->resc = resource;

  rhold->len = len;

  add_event(*event, node, TM_RESOURCES, (void *)rhold);

  return TM_SUCCESS;
  } /* END tm_rescinfo() */