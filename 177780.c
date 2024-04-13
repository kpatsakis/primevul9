dlmmap_locked (void *start, size_t length, int prot, int flags, off_t offset)
{
  void *ptr;

  if (execfd == -1)
    {
      open_temp_exec_file_opts_idx = 0;
    retry_open:
      execfd = open_temp_exec_file ();
      if (execfd == -1)
	return MFAIL;
    }

  offset = execsize;

  if (allocate_space (execfd, offset, length))
    return MFAIL;

  flags &= ~(MAP_PRIVATE | MAP_ANONYMOUS);
  flags |= MAP_SHARED;

  ptr = mmap (NULL, length, (prot & ~PROT_WRITE) | PROT_EXEC,
	      flags, execfd, offset);
  if (ptr == MFAIL)
    {
      if (!offset)
	{
	  close (execfd);
	  goto retry_open;
	}
      ftruncate (execfd, offset);
      return MFAIL;
    }
  else if (!offset
	   && open_temp_exec_file_opts[open_temp_exec_file_opts_idx].repeat)
    open_temp_exec_file_opts_next ();

  start = mmap (start, length, prot, flags, execfd, offset);

  if (start == MFAIL)
    {
      munmap (ptr, length);
      ftruncate (execfd, offset);
      return start;
    }

  mmap_exec_offset ((char *)start, length) = (char*)ptr - (char*)start;

  execsize += length;

  return start;
}