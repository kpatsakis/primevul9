_dbus_poll (DBusPollFD *fds,
            int         n_fds,
            int         timeout_milliseconds)
{
#if defined(HAVE_POLL) && !defined(BROKEN_POLL)
  /* DBusPollFD is a struct pollfd in this code path, so we can just poll() */
  if (timeout_milliseconds < -1)
    {
      timeout_milliseconds = -1;
    }

  return poll (fds,
               n_fds,
               timeout_milliseconds);
#else /* ! HAVE_POLL */
  /* Emulate poll() in terms of select() */
  fd_set read_set, write_set, err_set;
  int max_fd = 0;
  int i;
  struct timeval tv;
  int ready;

  FD_ZERO (&read_set);
  FD_ZERO (&write_set);
  FD_ZERO (&err_set);

  for (i = 0; i < n_fds; i++)
    {
      DBusPollFD *fdp = &fds[i];

      if (fdp->events & _DBUS_POLLIN)
	FD_SET (fdp->fd, &read_set);

      if (fdp->events & _DBUS_POLLOUT)
	FD_SET (fdp->fd, &write_set);

      FD_SET (fdp->fd, &err_set);

      max_fd = MAX (max_fd, fdp->fd);
    }

  tv.tv_sec = timeout_milliseconds / 1000;
  tv.tv_usec = (timeout_milliseconds % 1000) * 1000;

  ready = select (max_fd + 1, &read_set, &write_set, &err_set,
                  timeout_milliseconds < 0 ? NULL : &tv);

  if (ready > 0)
    {
      for (i = 0; i < n_fds; i++)
	{
	  DBusPollFD *fdp = &fds[i];

	  fdp->revents = 0;

	  if (FD_ISSET (fdp->fd, &read_set))
	    fdp->revents |= _DBUS_POLLIN;

	  if (FD_ISSET (fdp->fd, &write_set))
	    fdp->revents |= _DBUS_POLLOUT;

	  if (FD_ISSET (fdp->fd, &err_set))
	    fdp->revents |= _DBUS_POLLERR;
	}
    }

  return ready;
#endif
}