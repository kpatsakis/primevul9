int fake_tm_init(

  void             *info,  /* in, currently unused */
  struct  tm_roots *roots) /* out */

  {
  tm_event_t  nevent, revent;
  /*char   *env, *hold;*/
  /*int   err;*/
  int   nerr = 0;
  /*struct tcp_chan *chan = NULL;*/

  init_done = 1;

  nevent = new_event();

  add_event(nevent, TM_ERROR_NODE, TM_INIT, (void *)roots);
  tm_poll(TM_NULL_EVENT, &revent, 1, &nerr);
  return(0);
  }