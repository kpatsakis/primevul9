selinux_enabled_check (void)
{
  struct statfs sfs;
  FILE *f;
  char *buf = NULL;
  size_t len = 0;

  if (statfs ("/selinux", &sfs) >= 0
      && (unsigned int) sfs.f_type == 0xf97cff8cU)
    return 1;
  f = fopen ("/proc/mounts", "r");
  if (f == NULL)
    return 0;
  while (getline (&buf, &len, f) >= 0)
    {
      char *p = strchr (buf, ' ');
      if (p == NULL)
        break;
      p = strchr (p + 1, ' ');
      if (p == NULL)
        break;
      if (strncmp (p + 1, "selinuxfs ", 10) == 0)
        {
          free (buf);
          fclose (f);
          return 1;
        }
    }
  free (buf);
  fclose (f);
  return 0;
}