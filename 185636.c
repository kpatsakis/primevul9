flatpak_complete_build (FlatpakCompletion *completion)
{
  g_autoptr(GOptionContext) context = NULL;

  context = g_option_context_new ("");

  if (!flatpak_option_context_parse (context, options, &completion->argc, &completion->argv,
                                     FLATPAK_BUILTIN_FLAG_NO_DIR, NULL, NULL, NULL))
    return FALSE;

  switch (completion->argc)
    {
    case 0:
    case 1: /* DIR */
      flatpak_complete_options (completion, global_entries);
      flatpak_complete_options (completion, options);

      flatpak_complete_dir (completion);
      break;
    }

  return TRUE;
}