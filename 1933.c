flatpak_run_get_pulse_machine_id (void)
{
  static const char * const machine_ids[] =
  {
    "/etc/machine-id",
    "/var/lib/dbus/machine-id",
  };
  gsize i;

  for (i = 0; i < G_N_ELEMENTS (machine_ids); i++)
    {
      g_autofree char *ret = NULL;

      if (g_file_get_contents (machine_ids[i], &ret, NULL, NULL))
        {
          gsize j;

          g_strstrip (ret);

          for (j = 0; ret[j] != '\0'; j++)
            {
              if (!g_ascii_isxdigit (ret[j]))
                break;
            }

          if (ret[0] != '\0' && ret[j] == '\0')
            return g_steal_pointer (&ret);
        }
    }

  return g_strdup (g_get_host_name ());
}