add_dconf_key_to_keyfile (GKeyFile      *keyfile,
                          DConfClient   *client,
                          const char    *key,
                          DConfReadFlags flags)
{
  g_autofree char *group = g_path_get_dirname (key);
  g_autofree char *k = g_path_get_basename (key);
  GVariant *value = dconf_client_read_full (client, key, flags, NULL);

  if (value)
    {
      g_autofree char *val = g_variant_print (value, TRUE);
      g_key_file_set_value (keyfile, group + 1, k, val);
    }
}