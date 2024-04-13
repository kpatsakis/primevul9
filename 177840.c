int tm_atnode(

  tm_task_id  tid,  /* in  */
  tm_node_id *node)  /* out */

  {
  task_info *tp;

  if (!init_done)
    return TM_BADINIT;

  if ((tp = find_task(tid)) == NULL)
    return TM_ENOTFOUND;

  *node = tp->t_node;

  return TM_SUCCESS;
  }