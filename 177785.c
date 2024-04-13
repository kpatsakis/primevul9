dlmmap (void *start, size_t length, int prot,
	int flags, int fd, off_t offset)
{
  void *ptr;

  assert (start == NULL && length % malloc_getpagesize == 0
	  && prot == (PROT_READ | PROT_WRITE)
	  && flags == (MAP_PRIVATE | MAP_ANONYMOUS)
	  && fd == -1 && offset == 0);

  if (execfd == -1 && is_emutramp_enabled ())
    {
      ptr = mmap (start, length, prot & ~PROT_EXEC, flags, fd, offset);
      return ptr;
    }

  if (execfd == -1 && !is_selinux_enabled ())
    {
      ptr = mmap (start, length, prot | PROT_EXEC, flags, fd, offset);

      if (ptr != MFAIL || (errno != EPERM && errno != EACCES))
	/* Cool, no need to mess with separate segments.  */
	return ptr;

      /* If MREMAP_DUP is ever introduced and implemented, try mmap
	 with ((prot & ~PROT_WRITE) | PROT_EXEC) and mremap with
	 MREMAP_DUP and prot at this point.  */
    }

  if (execsize == 0 || execfd == -1)
    {
      pthread_mutex_lock (&open_temp_exec_file_mutex);
      ptr = dlmmap_locked (start, length, prot, flags, offset);
      pthread_mutex_unlock (&open_temp_exec_file_mutex);

      return ptr;
    }

  return dlmmap_locked (start, length, prot, flags, offset);
}