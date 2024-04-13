int tm_nodeinfo(

  tm_node_id  **list,
  int          *nnodes)

  {
  tm_node_id *np;
  int  i;
  int  n = 0;

  if (!init_done)
    {
    return (TM_BADINIT);
    }

  if (node_table == NULL)
    {
    return (TM_ESYSTEM);
    }

  for (np = node_table; *np != TM_ERROR_NODE; np++)
    n++;  /* how many nodes */

  if ((np = (tm_node_id *)calloc(n,sizeof(tm_node_id))) == NULL)
    {
    /* FAILURE - cannot alloc memory */

    return(TM_ERROR);
    }

  for (i = 0; i < n; i++)
    np[i] = node_table[i];

  *list = np;

  *nnodes = i;

  return(TM_SUCCESS);
  }  /* END tm_nodeinfo() */