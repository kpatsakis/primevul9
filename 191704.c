int lcc_network_buffer_set_security_level (lcc_network_buffer_t *nb, /* {{{ */
    lcc_security_level_t level,
    const char *username, const char *password)
{
  char *username_copy;
  char *password_copy;

  if (level == NONE)
  {
    free (nb->username);
    free (nb->password);
    nb->username = NULL;
    nb->password = NULL;
    nb->seclevel = NONE;
    lcc_network_buffer_initialize (nb);
    return (0);
  }

  if (!have_gcrypt ())
    return (ENOTSUP);

  username_copy = strdup (username);
  password_copy = strdup (password);
  if ((username_copy == NULL) || (password_copy == NULL))
  {
    free (username_copy);
    free (password_copy);
    return (ENOMEM);
  }

  free (nb->username);
  free (nb->password);
  nb->username = username_copy;
  nb->password = password_copy;
  nb->seclevel = level;

  lcc_network_buffer_initialize (nb);
  return (0);
} /* }}} int lcc_network_buffer_set_security_level */