grub_auth_check_authentication (const char *userlist)
{
  char login[1024];
  struct grub_auth_user *cur = NULL;
  static unsigned long punishment_delay = 1;
  char entered[GRUB_AUTH_MAX_PASSLEN];
  struct grub_auth_user *user;

  grub_memset (login, 0, sizeof (login));

  if (is_authenticated (userlist))
    {
      punishment_delay = 1;
      return GRUB_ERR_NONE;
    }

  grub_puts_ (N_("Enter username: "));

  if (!grub_username_get (login, sizeof (login) - 1))
    goto access_denied;

  grub_puts_ (N_("Enter password: "));

  if (!grub_password_get (entered, GRUB_AUTH_MAX_PASSLEN))
    goto access_denied;

  FOR_LIST_ELEMENTS (user, users)
    {
      if (grub_strcmp (login, user->name) == 0)
	cur = user;
    }

  if (!cur || ! cur->callback)
    goto access_denied;

  cur->callback (login, entered, cur->arg);
  if (is_authenticated (userlist))
    {
      punishment_delay = 1;
      return GRUB_ERR_NONE;
    }

 access_denied:
  grub_sleep (punishment_delay);

  if (punishment_delay < GRUB_ULONG_MAX / 2)
    punishment_delay *= 2;

  return GRUB_ACCESS_DENIED;
}