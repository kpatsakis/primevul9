_dbus_print_backtrace (void)
{
#if defined (HAVE_BACKTRACE) && defined (DBUS_BUILT_R_DYNAMIC)
  void *bt[500];
  int bt_size;
  int i;
  char **syms;

  bt_size = backtrace (bt, 500);

  syms = backtrace_symbols (bt, bt_size);

  i = 0;
  while (i < bt_size)
    {
      /* don't use dbus_warn since it can _dbus_abort() */
      fprintf (stderr, "  %s\n", syms[i]);
      ++i;
    }
  fflush (stderr);

  free (syms);
#elif defined (HAVE_BACKTRACE) && ! defined (DBUS_BUILT_R_DYNAMIC)
  fprintf (stderr, "  D-Bus not built with -rdynamic so unable to print a backtrace\n");
#else
  fprintf (stderr, "  D-Bus not compiled with backtrace support so unable to print a backtrace\n");
#endif
}