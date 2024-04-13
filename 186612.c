_dbus_read_credentials_socket  (DBusSocket       client_fd,
                                DBusCredentials *credentials,
                                DBusError       *error)
{
  struct msghdr msg;
  struct iovec iov;
  char buf;
  dbus_uid_t uid_read;
  dbus_gid_t primary_gid_read;
  dbus_pid_t pid_read;
  int bytes_read;

#ifdef HAVE_CMSGCRED
  union {
    struct cmsghdr hdr;
    char cred[CMSG_SPACE (sizeof (struct cmsgcred))];
  } cmsg;
#endif

  /* The POSIX spec certainly doesn't promise this, but
   * we need these assertions to fail as soon as we're wrong about
   * it so we can do the porting fixups
   */
  _DBUS_STATIC_ASSERT (sizeof (pid_t) <= sizeof (dbus_pid_t));
  _DBUS_STATIC_ASSERT (sizeof (uid_t) <= sizeof (dbus_uid_t));
  _DBUS_STATIC_ASSERT (sizeof (gid_t) <= sizeof (dbus_gid_t));

  uid_read = DBUS_UID_UNSET;
  primary_gid_read = DBUS_GID_UNSET;
  pid_read = DBUS_PID_UNSET;

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  _dbus_credentials_clear (credentials);

  iov.iov_base = &buf;
  iov.iov_len = 1;

  _DBUS_ZERO(msg);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

#if defined(HAVE_CMSGCRED)
  _DBUS_ZERO(cmsg);
  msg.msg_control = (caddr_t) &cmsg;
  msg.msg_controllen = CMSG_SPACE (sizeof (struct cmsgcred));
#endif

 again:
  bytes_read = recvmsg (client_fd.fd, &msg, 0);

  if (bytes_read < 0)
    {
      if (errno == EINTR)
	goto again;

      /* EAGAIN or EWOULDBLOCK would be unexpected here since we would
       * normally only call read_credentials if the socket was ready
       * for reading
       */

      dbus_set_error (error, _dbus_error_from_errno (errno),
                      "Failed to read credentials byte: %s",
                      _dbus_strerror (errno));
      return FALSE;
    }
  else if (bytes_read == 0)
    {
      /* this should not happen unless we are using recvmsg wrong,
       * so is essentially here for paranoia
       */
      dbus_set_error (error, DBUS_ERROR_FAILED,
                      "Failed to read credentials byte (zero-length read)");
      return FALSE;
    }
  else if (buf != '\0')
    {
      dbus_set_error (error, DBUS_ERROR_FAILED,
                      "Credentials byte was not nul");
      return FALSE;
    }

  _dbus_verbose ("read credentials byte\n");

  {
#ifdef SO_PEERCRED
    /* Supported by at least Linux and OpenBSD, with minor differences.
     *
     * This mechanism passes the process ID through and does not require
     * the peer's cooperation, so we prefer it over all others. Notably,
     * Linux also supports SCM_CREDENTIALS, which is similar to FreeBSD
     * SCM_CREDS; it's implemented in GIO, but we don't use it in dbus at all,
     * because this is much less fragile.
     */
#ifdef __OpenBSD__
    struct sockpeercred cr;
#else
    struct ucred cr;
#endif
    socklen_t cr_len = sizeof (cr);

    if (getsockopt (client_fd.fd, SOL_SOCKET, SO_PEERCRED, &cr, &cr_len) != 0)
      {
        _dbus_verbose ("Failed to getsockopt(SO_PEERCRED): %s\n",
                       _dbus_strerror (errno));
      }
    else if (cr_len != sizeof (cr))
      {
        _dbus_verbose ("Failed to getsockopt(SO_PEERCRED), returned %d bytes, expected %d\n",
                       cr_len, (int) sizeof (cr));
      }
    else
      {
        pid_read = cr.pid;
        uid_read = cr.uid;
#ifdef __linux__
        /* Do other platforms have cr.gid? (Not that it really matters,
         * because the gid is useless to us unless we know the complete
         * group vector, which we only know on Linux.) */
        primary_gid_read = cr.gid;
#endif
      }
#elif defined(HAVE_UNPCBID) && defined(LOCAL_PEEREID)
    /* Another variant of the above - used on NetBSD
     */
    struct unpcbid cr;
    socklen_t cr_len = sizeof (cr);

    if (getsockopt (client_fd.fd, 0, LOCAL_PEEREID, &cr, &cr_len) != 0)
      {
        _dbus_verbose ("Failed to getsockopt(LOCAL_PEEREID): %s\n",
                       _dbus_strerror (errno));
      }
    else if (cr_len != sizeof (cr))
      {
        _dbus_verbose ("Failed to getsockopt(LOCAL_PEEREID), returned %d bytes, expected %d\n",
                       cr_len, (int) sizeof (cr));
      }
    else
      {
        pid_read = cr.unp_pid;
        uid_read = cr.unp_euid;
      }
#elif defined(HAVE_CMSGCRED)
    /* We only check for HAVE_CMSGCRED, but we're really assuming that the
     * presence of that struct implies SCM_CREDS. Supported by at least
     * FreeBSD and DragonflyBSD.
     *
     * This mechanism requires the peer to help us (it has to send us a
     * SCM_CREDS message) but it does pass the process ID through,
     * which makes it better than getpeereid().
     */
    struct cmsgcred *cred;
    struct cmsghdr *cmsgp;

    for (cmsgp = CMSG_FIRSTHDR (&msg);
         cmsgp != NULL;
         cmsgp = CMSG_NXTHDR (&msg, cmsgp))
      {
        if (cmsgp->cmsg_type == SCM_CREDS &&
            cmsgp->cmsg_level == SOL_SOCKET &&
            cmsgp->cmsg_len >= CMSG_LEN (sizeof (struct cmsgcred)))
          {
            cred = (struct cmsgcred *) CMSG_DATA (cmsgp);
            pid_read = cred->cmcred_pid;
            uid_read = cred->cmcred_euid;
            break;
          }
      }

#elif defined(HAVE_GETPEERUCRED)
    /* Supported in at least Solaris >= 10. It should probably be higher
     * up this list, because it carries the pid and we use this code path
     * for audit data. */
    ucred_t * ucred = NULL;
    if (getpeerucred (client_fd.fd, &ucred) == 0)
      {
#ifdef HAVE_ADT
        adt_session_data_t *adth = NULL;
#endif
        pid_read = ucred_getpid (ucred);
        uid_read = ucred_geteuid (ucred);
#ifdef HAVE_ADT
        /* generate audit session data based on socket ucred */
        if (adt_start_session (&adth, NULL, 0) || (adth == NULL))
          {
            _dbus_verbose ("Failed to adt_start_session(): %s\n", _dbus_strerror (errno));
          }
        else
          {
            if (adt_set_from_ucred (adth, ucred, ADT_NEW))
              {
                _dbus_verbose ("Failed to adt_set_from_ucred(): %s\n", _dbus_strerror (errno));
              }
            else
              {
                adt_export_data_t *data = NULL;
                size_t size = adt_export_session_data (adth, &data);
                if (size <= 0)
                  {
                    _dbus_verbose ("Failed to adt_export_session_data(): %s\n", _dbus_strerror (errno));
                  }
                else
                  {
                    _dbus_credentials_add_adt_audit_data (credentials, data, size);
                    free (data);
                  }
              }
            (void) adt_end_session (adth);
          }
#endif /* HAVE_ADT */
      }
    else
      {
        _dbus_verbose ("Failed to getpeerucred() credentials: %s\n", _dbus_strerror (errno));
      }
    if (ucred != NULL)
      ucred_free (ucred);

    /* ----------------------------------------------------------------
     * When adding new mechanisms, please add them above this point
     * if they support passing the process ID through, or below if not.
     * ---------------------------------------------------------------- */

#elif defined(HAVE_GETPEEREID)
    /* getpeereid() originates from D.J. Bernstein and is fairly
     * widely-supported. According to a web search, it might be present in
     * any/all of:
     *
     * - AIX?
     * - Blackberry?
     * - Cygwin
     * - FreeBSD 4.6+ (but we prefer SCM_CREDS: it carries the pid)
     * - Mac OS X
     * - Minix 3.1.8+
     * - MirBSD?
     * - NetBSD 5.0+ (but LOCAL_PEEREID would be better: it carries the pid)
     * - OpenBSD 3.0+ (but we prefer SO_PEERCRED: it carries the pid)
     * - QNX?
     */
    uid_t euid;
    gid_t egid;
    if (getpeereid (client_fd.fd, &euid, &egid) == 0)
      {
        uid_read = euid;
      }
    else
      {
        _dbus_verbose ("Failed to getpeereid() credentials: %s\n", _dbus_strerror (errno));
      }
#else /* no supported mechanism */

#warning Socket credentials not supported on this Unix OS
#warning Please tell https://bugs.freedesktop.org/enter_bug.cgi?product=DBus

    /* Please add other operating systems known to support at least one of
     * the mechanisms above to this list, keeping alphabetical order.
     * Everything not in this list  is best-effort.
     */
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || \
    defined(__linux__) || \
    defined(__OpenBSD__) || \
    defined(__NetBSD__)
# error Credentials passing not working on this OS is a regression!
#endif

    _dbus_verbose ("Socket credentials not supported on this OS\n");
#endif
  }

  _dbus_verbose ("Credentials:"
                 "  pid "DBUS_PID_FORMAT
                 "  uid "DBUS_UID_FORMAT
                 "\n",
		 pid_read,
		 uid_read);

  if (pid_read != DBUS_PID_UNSET)
    {
      if (!_dbus_credentials_add_pid (credentials, pid_read))
        {
          _DBUS_SET_OOM (error);
          return FALSE;
        }
    }

  if (uid_read != DBUS_UID_UNSET)
    {
      if (!_dbus_credentials_add_unix_uid (credentials, uid_read))
        {
          _DBUS_SET_OOM (error);
          return FALSE;
        }
    }

  if (!add_linux_security_label_to_credentials (client_fd.fd, credentials))
    {
      _DBUS_SET_OOM (error);
      return FALSE;
    }

  /* We don't put any groups in the credentials unless we can put them
   * all there. */
  if (!add_groups_to_credentials (client_fd.fd, credentials, primary_gid_read))
    {
      _DBUS_SET_OOM (error);
      return FALSE;
    }

  return TRUE;
}