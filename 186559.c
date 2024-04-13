fill_user_info (DBusUserInfo       *info,
                dbus_uid_t          uid,
                const DBusString   *username,
                DBusError          *error)
{
  const char *username_c;

  /* exactly one of username/uid provided */
  _dbus_assert (username != NULL || uid != DBUS_UID_UNSET);
  _dbus_assert (username == NULL || uid == DBUS_UID_UNSET);

  info->uid = DBUS_UID_UNSET;
  info->primary_gid = DBUS_GID_UNSET;
  info->group_ids = NULL;
  info->n_group_ids = 0;
  info->username = NULL;
  info->homedir = NULL;

  if (username != NULL)
    username_c = _dbus_string_get_const_data (username);
  else
    username_c = NULL;

  /* For now assuming that the getpwnam() and getpwuid() flavors
   * are always symmetrical, if not we have to add more configure
   * checks
   */

#ifdef HAVE_GETPWNAM_R
  {
    struct passwd *p;
    int result;
    size_t buflen;
    char *buf;
    struct passwd p_str;

    /* retrieve maximum needed size for buf */
    buflen = sysconf (_SC_GETPW_R_SIZE_MAX);

    /* sysconf actually returns a long, but everything else expects size_t,
     * so just recast here.
     * https://bugs.freedesktop.org/show_bug.cgi?id=17061
     */
    if ((long) buflen <= 0)
      buflen = 1024;

    result = -1;
    while (1)
      {
        buf = dbus_malloc (buflen);
        if (buf == NULL)
          {
            dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
            return FALSE;
          }

        p = NULL;
        if (uid != DBUS_UID_UNSET)
          result = getpwuid_r (uid, &p_str, buf, buflen,
                               &p);
        else
          result = getpwnam_r (username_c, &p_str, buf, buflen,
                               &p);
        //Try a bigger buffer if ERANGE was returned
        if (result == ERANGE && buflen < 512 * 1024)
          {
            dbus_free (buf);
            buflen *= 2;
          }
        else
          {
            break;
          }
      }
    if (result == 0 && p == &p_str)
      {
        if (!fill_user_info_from_passwd (p, info, error))
          {
            dbus_free (buf);
            return FALSE;
          }
        dbus_free (buf);
      }
    else
      {
        dbus_set_error (error, _dbus_error_from_errno (errno),
                        "User \"%s\" unknown or no memory to allocate password entry\n",
                        username_c ? username_c : "???");
        _dbus_verbose ("User %s unknown\n", username_c ? username_c : "???");
        dbus_free (buf);
        return FALSE;
      }
  }
#else /* ! HAVE_GETPWNAM_R */
  {
    /* I guess we're screwed on thread safety here */
    struct passwd *p;

#warning getpwnam_r() not available, please report this to the dbus maintainers with details of your OS

    if (uid != DBUS_UID_UNSET)
      p = getpwuid (uid);
    else
      p = getpwnam (username_c);

    if (p != NULL)
      {
        if (!fill_user_info_from_passwd (p, info, error))
          {
            return FALSE;
          }
      }
    else
      {
        dbus_set_error (error, _dbus_error_from_errno (errno),
                        "User \"%s\" unknown or no memory to allocate password entry\n",
                        username_c ? username_c : "???");
        _dbus_verbose ("User %s unknown\n", username_c ? username_c : "???");
        return FALSE;
      }
  }
#endif  /* ! HAVE_GETPWNAM_R */

  /* Fill this in so we can use it to get groups */
  username_c = info->username;

#ifdef HAVE_GETGROUPLIST
  {
    gid_t *buf;
    int buf_count;
    int i;
    int initial_buf_count;

    initial_buf_count = 17;
    buf_count = initial_buf_count;
    buf = dbus_new (gid_t, buf_count);
    if (buf == NULL)
      {
        dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
        goto failed;
      }

    if (getgrouplist (username_c,
                      info->primary_gid,
                      buf, &buf_count) < 0)
      {
        gid_t *new;
        /* Presumed cause of negative return code: buf has insufficient
           entries to hold the entire group list. The Linux behavior in this
           case is to pass back the actual number of groups in buf_count, but
           on Mac OS X 10.5, buf_count is unhelpfully left alone.
           So as a hack, try to help out a bit by guessing a larger
           number of groups, within reason.. might still fail, of course,
           but we can at least print a more informative message.  I looked up
           the "right way" to do this by downloading Apple's own source code
           for the "id" command, and it turns out that they use an
           undocumented library function getgrouplist_2 (!) which is not
           declared in any header in /usr/include (!!). That did not seem
           like the way to go here.
        */
        if (buf_count == initial_buf_count)
          {
            buf_count *= 16; /* Retry with an arbitrarily scaled-up array */
          }
        new = dbus_realloc (buf, buf_count * sizeof (buf[0]));
        if (new == NULL)
          {
            dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
            dbus_free (buf);
            goto failed;
          }

        buf = new;

        errno = 0;
        if (getgrouplist (username_c, info->primary_gid, buf, &buf_count) < 0)
          {
            if (errno == 0)
              {
                _dbus_warn ("It appears that username \"%s\" is in more than %d groups.\nProceeding with just the first %d groups.",
                            username_c, buf_count, buf_count);
              }
            else
              {
                dbus_set_error (error,
                                _dbus_error_from_errno (errno),
                                "Failed to get groups for username \"%s\" primary GID "
                                DBUS_GID_FORMAT ": %s\n",
                                username_c, info->primary_gid,
                                _dbus_strerror (errno));
                dbus_free (buf);
                goto failed;
              }
          }
      }

    info->group_ids = dbus_new (dbus_gid_t, buf_count);
    if (info->group_ids == NULL)
      {
        dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
        dbus_free (buf);
        goto failed;
      }

    for (i = 0; i < buf_count; ++i)
      info->group_ids[i] = buf[i];

    info->n_group_ids = buf_count;

    dbus_free (buf);
  }
#else  /* HAVE_GETGROUPLIST */
  {
    /* We just get the one group ID */
    info->group_ids = dbus_new (dbus_gid_t, 1);
    if (info->group_ids == NULL)
      {
        dbus_set_error (error, DBUS_ERROR_NO_MEMORY, NULL);
        goto failed;
      }

    info->n_group_ids = 1;

    (info->group_ids)[0] = info->primary_gid;
  }
#endif /* HAVE_GETGROUPLIST */

  _DBUS_ASSERT_ERROR_IS_CLEAR (error);

  return TRUE;

 failed:
  _DBUS_ASSERT_ERROR_IS_SET (error);
  return FALSE;
}