int tm_spawn(

  int          argc,  /* in  */
  char       **argv,  /* in  */
  char       **envp,  /* in  */
  tm_node_id   where, /* in  */
  tm_task_id  *tid,   /* out */
  tm_event_t  *event) /* out */

  {
  int rc = TM_SUCCESS;
  char *cp;
  int   i;
  struct tcp_chan *chan = NULL;

  /* NOTE: init_done is global */

  if (!init_done)
    {
    return(TM_BADINIT);
    }

  if ((argc <= 0) || (argv == NULL) || (argv[0] == NULL) || (*argv[0] == '\0'))
    {
    return(TM_ENOTFOUND);
    }

  *event = new_event();

  if (startcom(TM_SPAWN, *event, &chan) != DIS_SUCCESS)
    {
    return(TM_ENOTCONNECTED);
    }

  if (diswsi(chan, where) != DIS_SUCCESS) /* send where */
    {
    rc = TM_ENOTCONNECTED;
    goto tm_spawn_cleanup;
    }

  if (diswsi(chan, argc) != DIS_SUCCESS) /* send argc */
    {
     rc = TM_ENOTCONNECTED;
     goto tm_spawn_cleanup;
    }

  /* send argv strings across */

  for (i = 0;i < argc;i++)
    {
    cp = argv[i];

    if (diswcs(chan, cp, strlen(cp)) != DIS_SUCCESS)
      {
       rc = TM_ENOTCONNECTED;
       goto tm_spawn_cleanup;
      }
    }

  /* send envp strings across */

  if (getenv("PBSDEBUG") != NULL)
    {
    if (diswcs(chan, "PBSDEBUG=1", strlen("PBSDEBUG=1")) != DIS_SUCCESS)
      {
       rc = TM_ENOTCONNECTED;
       goto tm_spawn_cleanup;
      }
    }

  if (envp != NULL)
    {
    for (i = 0;(cp = envp[i]) != NULL;i++)
      {
      if (diswcs(chan, cp, strlen(cp)) != DIS_SUCCESS)
        {
         rc = TM_ENOTCONNECTED;
         goto tm_spawn_cleanup;
        }
      }
    }

  if (diswcs(chan, "", 0) != DIS_SUCCESS)
    {
     rc = TM_ENOTCONNECTED;
     goto tm_spawn_cleanup;
    }

  DIS_tcp_wflush(chan);

  add_event(*event, where, TM_SPAWN, (void *)tid);

tm_spawn_cleanup:
  if (chan != NULL)
    DIS_tcp_cleanup(chan);

  return(rc);
  }  /* END tm_spawn() */