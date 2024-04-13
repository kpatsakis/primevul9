flatpak_run_add_cups_args (FlatpakBwrap *bwrap)
{
  g_autofree char * sandbox_server_name = g_strdup ("/var/run/cups/cups.sock");
  g_autofree char * cups_server_name = flatpak_run_get_cups_server_name ();

  if (!g_file_test (cups_server_name, G_FILE_TEST_EXISTS))
    {
      g_debug ("Could not find CUPS server");
      return;
    }

  flatpak_bwrap_add_args (bwrap,
                          "--ro-bind", cups_server_name, sandbox_server_name,
                          NULL);
}