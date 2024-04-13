is_authenticated (const char *userlist)
{
  const char *superusers;
  struct grub_auth_user *user;

  superusers = grub_env_get ("superusers");

  if (!superusers)
    return 1;

  FOR_LIST_ELEMENTS (user, users)
    {
      if (!(user->authenticated))
	continue;

      if ((userlist && grub_strword (userlist, user->name))
	  || grub_strword (superusers, user->name))
	return 1;
    }

  return 0;
}