flatpak_app_compute_permissions (GKeyFile *app_metadata,
                                 GKeyFile *runtime_metadata,
                                 GError  **error)
{
  g_autoptr(FlatpakContext) app_context = NULL;

  app_context = flatpak_context_new ();

  if (runtime_metadata != NULL)
    {
      if (!flatpak_context_load_metadata (app_context, runtime_metadata, error))
        return NULL;

      /* Don't inherit any permissions from the runtime, only things like env vars. */
      flatpak_context_reset_permissions (app_context);
    }

  if (app_metadata != NULL &&
      !flatpak_context_load_metadata (app_context, app_metadata, error))
    return NULL;

  return g_steal_pointer (&app_context);
}