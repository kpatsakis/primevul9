write_credentials_byte (int             server_fd,
                        DBusError      *error)
{
  int bytes_written;
  char buf[1] = { '\0' };
#if defined(HAVE_CMSGCRED)
  union {
	  struct cmsghdr hdr;
	  char cred[CMSG_SPACE (sizeof (struct cmsgcred))];
  } cmsg;
  struct iovec iov;
  struct msghdr msg;
  iov.iov_base = buf;
  iov.iov_len = 1;

  _DBUS_ZERO(msg);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  msg.msg_control = (caddr_t) &cmsg;
  msg.msg_controllen = CMSG_SPACE (sizeof (struct cmsgcred));
  _DBUS_ZERO(cmsg);
  cmsg.hdr.cmsg_len = CMSG_LEN (sizeof (struct cmsgcred));
  cmsg.hdr.cmsg_level = SOL_SOCKET;
  cmsg.hdr.cmsg_type = SCM_CREDS;
#endif

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

 again:

#if defined(HAVE_CMSGCRED)
  bytes_written = sendmsg (server_fd, &msg, 0
#if HAVE_DECL_MSG_NOSIGNAL
                           |MSG_NOSIGNAL
#endif
                           );

  /* If we HAVE_CMSGCRED, the OS still might not let us sendmsg()
   * with a SOL_SOCKET/SCM_CREDS message - for instance, FreeBSD
   * only allows that on AF_UNIX. Try just doing a send() instead. */
  if (bytes_written < 0 && errno == EINVAL)
#endif
    {
      bytes_written = send (server_fd, buf, 1, 0
#if HAVE_DECL_MSG_NOSIGNAL
                            |MSG_NOSIGNAL
#endif
                            );
    }

  if (bytes_written < 0 && errno == EINTR)
    goto again;

  if (bytes_written < 0)
    {
      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to write credentials byte: %s",
                     _dbus_strerror (errno));
      return FALSE;
    }
  else if (bytes_written == 0)
    {
      dbus_set_error (error, DBUS_ERROR_IO_ERROR,
                      "wrote zero bytes writing credentials byte");
      return FALSE;
    }
  else
    {
      _dbus_assert (bytes_written == 1);
      _dbus_verbose ("wrote credentials byte\n");
      return TRUE;
    }
}