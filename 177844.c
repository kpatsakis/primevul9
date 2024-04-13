task_info *find_task(
    
  tm_task_id x)

  {
  task_info *tp;

  for (tp = task_hash[x % TASK_HASH]; tp; tp = tp->t_next)
    {
    if (tp->t_task == x)
      break;
    }

  return tp;
  }