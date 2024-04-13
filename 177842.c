tm_task_id new_task(
    
  char       *jobid,
  tm_node_id  node,
  tm_task_id  task)

  {
  task_info  *tp, **head;

  TM_DBPRT(("%s: jobid=%s node=%d task=%lu\n",
         __func__, jobid, node, (unsigned long)task))

  if (jobid != tm_jobid && strcmp(jobid, tm_jobid) != 0)
    {
    TM_DBPRT(("%s: task job %s not my job %s\n",
           __func__, jobid, tm_jobid))
    return TM_NULL_TASK;
    }

  if (node == TM_ERROR_NODE)
    {
    TM_DBPRT(("%s: called with TM_ERROR_NODE\n", __func__))
    return TM_NULL_TASK;
    }

  if ((tp = find_task(task)) != NULL)
    {
    TM_DBPRT(("%s: task %lu found with node %d should be %d\n",
           __func__, (unsigned long)task, tp->t_node, node))
    return task;
    }

  if ((tp = (task_info *)calloc(1, sizeof(task_info))) == NULL)
    return TM_NULL_TASK;

  head = &task_hash[task % TASK_HASH];

  tp->t_jobid = tm_jobid;

  tp->t_task  = task;

  tp->t_node  = node;

  tp->t_next = *head;

  *head = tp;

  return task;
  }