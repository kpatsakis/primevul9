flatpak_run_in_transient_unit (const char *appid, GError **error)
{
  g_autoptr(GDBusConnection) conn = NULL;
  g_autofree char *path = NULL;
  g_autofree char *address = NULL;
  g_autofree char *name = NULL;
  g_autofree char *appid_escaped = NULL;
  g_autofree char *job = NULL;
  SystemdManager *manager = NULL;
  GVariantBuilder builder;
  GVariant *properties = NULL;
  GVariant *aux = NULL;
  guint32 pid;
  GMainLoop *main_loop = NULL;
  struct JobData data;
  gboolean res = FALSE;
  g_autoptr(GMainContextPopDefault) main_context = NULL;

  path = g_strdup_printf ("/run/user/%d/systemd/private", getuid ());

  if (!g_file_test (path, G_FILE_TEST_EXISTS))
    return flatpak_fail_error (error, FLATPAK_ERROR_SETUP_FAILED,
                               _("No systemd user session available, cgroups not available"));

  main_context = flatpak_main_context_new_default ();
  main_loop = g_main_loop_new (main_context, FALSE);

  address = g_strconcat ("unix:path=", path, NULL);

  conn = g_dbus_connection_new_for_address_sync (address,
                                                 G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT,
                                                 NULL,
                                                 NULL, error);
  if (!conn)
    goto out;

  manager = systemd_manager_proxy_new_sync (conn,
                                            G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
                                            NULL,
                                            "/org/freedesktop/systemd1",
                                            NULL, error);
  if (!manager)
    goto out;

  appid_escaped = systemd_unit_name_escape (appid);
  name = g_strdup_printf ("app-flatpak-%s-%d.scope", appid_escaped, getpid ());

  g_variant_builder_init (&builder, G_VARIANT_TYPE ("a(sv)"));

  pid = getpid ();
  g_variant_builder_add (&builder, "(sv)",
                         "PIDs",
                         g_variant_new_fixed_array (G_VARIANT_TYPE ("u"),
                                                    &pid, 1, sizeof (guint32))
                        );

  properties = g_variant_builder_end (&builder);

  aux = g_variant_new_array (G_VARIANT_TYPE ("(sa(sv))"), NULL, 0);

  if (!systemd_manager_call_start_transient_unit_sync (manager,
                                                       name,
                                                       "fail",
                                                       properties,
                                                       aux,
                                                       &job,
                                                       NULL,
                                                       error))
    goto out;

  data.job = job;
  data.main_loop = main_loop;
  g_signal_connect (manager, "job-removed", G_CALLBACK (job_removed_cb), &data);

  g_main_loop_run (main_loop);

  res = TRUE;

out:
  if (main_loop)
    g_main_loop_unref (main_loop);
  if (manager)
    g_object_unref (manager);

  return res;
}