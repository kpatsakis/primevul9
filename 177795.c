open_temp_exec_file (void)
{
  int fd;

  do
    {
      fd = open_temp_exec_file_opts[open_temp_exec_file_opts_idx].func
	(open_temp_exec_file_opts[open_temp_exec_file_opts_idx].arg);

      if (!open_temp_exec_file_opts[open_temp_exec_file_opts_idx].repeat
	  || fd == -1)
	{
	  if (open_temp_exec_file_opts_next ())
	    break;
	}
    }
  while (fd == -1);

  return fd;
}