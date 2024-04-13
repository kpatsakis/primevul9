write_xauth (char *number, FILE *output)
{
  Xauth *xa, local_xa;
  char *filename;
  FILE *f;
  struct utsname unames;

  if (uname (&unames))
    {
      g_warning ("uname failed");
      return;
    }

  filename = XauFileName ();
  f = fopen (filename, "rb");
  if (f == NULL)
    return;

  while (TRUE)
    {
      xa = XauReadAuth (f);
      if (xa == NULL)
        break;
      if (xauth_entry_should_propagate (xa, unames.nodename, number))
        {
          local_xa = *xa;
          if (local_xa.number)
            {
              local_xa.number = "99";
              local_xa.number_length = 2;
            }

          if (local_xa.family == FamilyLocal &&
              !auth_streq (unames.nodename, local_xa.address, local_xa.address_length))
            {
              /* If we decided to propagate this cookie, but its address
               * doesn't match `unames.nodename`, we need to change it or
               * inside the container it will not work.
               */
              local_xa.address = unames.nodename;
              local_xa.address_length = strlen (local_xa.address);
            }

          if (!XauWriteAuth (output, &local_xa))
            g_warning ("xauth write error");
        }

      XauDisposeAuth (xa);
    }

  fclose (f);
}