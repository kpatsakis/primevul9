flatpak_run_get_cups_server_name_config (const char *path)
{
  g_autoptr(GFile) file = g_file_new_for_path (path);
  g_autoptr(GError) my_error = NULL;
  g_autoptr(GFileInputStream) input_stream = NULL;
  g_autoptr(GDataInputStream) data_stream = NULL;
  size_t len;

  input_stream = g_file_read (file, NULL, &my_error);
  if (my_error)
    {
      g_debug ("CUPS configuration file '%s': %s", path, my_error->message);
      return NULL;
    }

  data_stream = g_data_input_stream_new (G_INPUT_STREAM (input_stream));

  while (TRUE)
    {
      g_autofree char *line = g_data_input_stream_read_line (data_stream, &len, NULL, NULL);
      if (line == NULL)
        break;

      g_strchug (line);

      if ((*line  == '\0') || (*line == '#'))
        continue;

      g_auto(GStrv) tokens = g_strsplit (line, " ", 2);

      if ((tokens[0] != NULL) && (tokens[1] != NULL))
        {
          if (strcmp ("ServerName", tokens[0]) == 0)
            {
              g_strchug (tokens[1]);

              if (flatpak_run_cups_check_server_is_socket (tokens[1]))
                return g_strdup (tokens[1]);
            }
        }
    }

    return NULL;
}