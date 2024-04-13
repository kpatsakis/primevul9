emutramp_enabled_check (void)
{
  char *buf = NULL;
  size_t len = 0;
  FILE *f;
  int ret;
  f = fopen ("/proc/self/status", "r");
  if (f == NULL)
    return 0;
  ret = 0;

  while (getline (&buf, &len, f) != -1)
    if (!strncmp (buf, "PaX:", 4))
      {
        char emutramp;
        if (sscanf (buf, "%*s %*c%c", &emutramp) == 1)
          ret = (emutramp == 'E');
        break;
      }
  free (buf);
  fclose (f);
  return ret;
}