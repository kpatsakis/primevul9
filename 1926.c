check_parental_controls (FlatpakDecomposed *app_ref,
                         FlatpakDeploy     *deploy,
                         GCancellable      *cancellable,
                         GError           **error)
{
#ifdef HAVE_LIBMALCONTENT
  g_autoptr(MctManager) manager = NULL;
  g_autoptr(MctAppFilter) app_filter = NULL;
  g_autoptr(GDBusConnection) system_bus = NULL;
  g_autoptr(GError) local_error = NULL;
  g_autoptr(GDesktopAppInfo) app_info = NULL;
  gboolean allowed = FALSE;

  system_bus = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, error);
  if (system_bus == NULL)
    return FALSE;

  manager = mct_manager_new (system_bus);
  app_filter = mct_manager_get_app_filter (manager, getuid (),
                                           MCT_GET_APP_FILTER_FLAGS_INTERACTIVE,
                                           cancellable, &local_error);
  if (g_error_matches (local_error, MCT_APP_FILTER_ERROR, MCT_APP_FILTER_ERROR_DISABLED))
    {
      g_debug ("Skipping parental controls check for %s since parental "
               "controls are disabled globally", flatpak_decomposed_get_ref (app_ref));
      return TRUE;
    }
  else if (g_error_matches (local_error, G_DBUS_ERROR, G_DBUS_ERROR_SERVICE_UNKNOWN) ||
           g_error_matches (local_error, G_DBUS_ERROR, G_DBUS_ERROR_NAME_HAS_NO_OWNER))
    {
      g_debug ("Skipping parental controls check for %s since a required "
               "service was not found", flatpak_decomposed_get_ref (app_ref));
      return TRUE;
    }
  else if (local_error != NULL)
    {
      g_propagate_error (error, g_steal_pointer (&local_error));
      return FALSE;
    }

  /* Always filter by app ID. Additionally, filter by app info (which runs
   * multiple checks, including whether the app ID, executable path and
   * content types are allowed) if available. If the flatpak contains
   * multiple .desktop files, we use the main one. The app ID check is
   * always done, as the binary executed by `flatpak run` isn’t necessarily
   * extracted from a .desktop file. */
  allowed = mct_app_filter_is_flatpak_ref_allowed (app_filter, flatpak_decomposed_get_ref (app_ref));

  /* Look up the app’s main .desktop file. */
  if (deploy != NULL && allowed)
    {
      g_autoptr(GFile) deploy_dir = NULL;
      const char *deploy_path;
      g_autofree char *desktop_file_name = NULL;
      g_autofree char *desktop_file_path = NULL;
      g_autofree char *app_id = flatpak_decomposed_dup_id (app_ref);

      deploy_dir = flatpak_deploy_get_dir (deploy);
      deploy_path = flatpak_file_get_path_cached (deploy_dir);

      desktop_file_name = g_strconcat (app_id, ".desktop", NULL);
      desktop_file_path = g_build_path (G_DIR_SEPARATOR_S,
                                        deploy_path,
                                        "export",
                                        "share",
                                        "applications",
                                        desktop_file_name,
                                        NULL);
      app_info = g_desktop_app_info_new_from_filename (desktop_file_path);
    }

  if (app_info != NULL)
    allowed = allowed && mct_app_filter_is_appinfo_allowed (app_filter,
                                                            G_APP_INFO (app_info));

  if (!allowed)
    return flatpak_fail_error (error, FLATPAK_ERROR_PERMISSION_DENIED,
                               /* Translators: The placeholder is for an app ref. */
                               _("Running %s is not allowed by the policy set by your administrator"),
                               flatpak_decomposed_get_ref (app_ref));
#endif  /* HAVE_LIBMALCONTENT */

  return TRUE;
}