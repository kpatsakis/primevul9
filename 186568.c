_dbus_write_socket_with_unix_fds_two(DBusSocket        fd,
                                     const DBusString *buffer1,
                                     int               start1,
                                     int               len1,
                                     const DBusString *buffer2,
                                     int               start2,
                                     int               len2,
                                     const int        *fds,
                                     int               n_fds) {

#ifndef HAVE_UNIX_FD_PASSING

  if (n_fds > 0) {
    errno = ENOTSUP;
    return -1;
  }

  return _dbus_write_socket_two(fd,
                                buffer1, start1, len1,
                                buffer2, start2, len2);
#else

  struct msghdr m;
  struct cmsghdr *cm;
  struct iovec iov[2];
  int bytes_written;

  _dbus_assert (len1 >= 0);
  _dbus_assert (len2 >= 0);
  _dbus_assert (n_fds >= 0);

  _DBUS_ZERO(iov);
  iov[0].iov_base = (char*) _dbus_string_get_const_data_len (buffer1, start1, len1);
  iov[0].iov_len = len1;

  if (buffer2)
    {
      iov[1].iov_base = (char*) _dbus_string_get_const_data_len (buffer2, start2, len2);
      iov[1].iov_len = len2;
    }

  _DBUS_ZERO(m);
  m.msg_iov = iov;
  m.msg_iovlen = buffer2 ? 2 : 1;

  if (n_fds > 0)
    {
      m.msg_controllen = CMSG_SPACE(n_fds * sizeof(int));
      m.msg_control = alloca(m.msg_controllen);
      memset(m.msg_control, 0, m.msg_controllen);

      cm = CMSG_FIRSTHDR(&m);
      cm->cmsg_level = SOL_SOCKET;
      cm->cmsg_type = SCM_RIGHTS;
      cm->cmsg_len = CMSG_LEN(n_fds * sizeof(int));
      memcpy(CMSG_DATA(cm), fds, n_fds * sizeof(int));
    }

 again:

  bytes_written = sendmsg (fd.fd, &m, 0
#if HAVE_DECL_MSG_NOSIGNAL
                           |MSG_NOSIGNAL
#endif
                           );

  if (bytes_written < 0 && errno == EINTR)
    goto again;

#if 0
  if (bytes_written > 0)
    _dbus_verbose_bytes_of_string (buffer, start, bytes_written);
#endif

  return bytes_written;
#endif
}