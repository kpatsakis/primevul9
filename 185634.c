find_matching_extension_group_in_metakey (GKeyFile   *metakey,
                                          const char *id,
                                          const char *specified_tag,
                                          char      **out_extension_group,
                                          GError    **error)
{
  g_auto(GStrv) groups = NULL;
  g_autofree char *extension_prefix = NULL;
  const char *last_seen_group = NULL;
  guint n_extension_groups = 0;
  GStrv iter = NULL;

  g_return_val_if_fail (out_extension_group != NULL, FALSE);

  groups =  g_key_file_get_groups (metakey, NULL);
  extension_prefix = g_strconcat (FLATPAK_METADATA_GROUP_PREFIX_EXTENSION,
                                  id,
                                  NULL);

  for (iter = groups; *iter != NULL; ++iter)
    {
      const char *group_name = *iter;
      const char *extension_name = NULL;
      g_autofree char *extension_tag = NULL;

      if (!g_str_has_prefix (group_name, extension_prefix))
        continue;

      ++n_extension_groups;
      extension_name = group_name + strlen (FLATPAK_METADATA_GROUP_PREFIX_EXTENSION);

      flatpak_parse_extension_with_tag (extension_name,
                                        NULL,
                                        &extension_tag);

      /* Check 1: Does this extension have the same tag as the
       * specified tag (including if both are NULL)? If so, use it */
      if (g_strcmp0 (extension_tag, specified_tag) == 0)
        {
          *out_extension_group = g_strdup (group_name);
          return TRUE;
        }

      /* Check 2: Keep track of this extension group as the last
       * seen one. If it was the only one then we can use it. */
      last_seen_group = group_name;
    }

  if (n_extension_groups == 1 && last_seen_group != NULL)
    {
      *out_extension_group = g_strdup (last_seen_group);
      return TRUE;
    }
  else if (n_extension_groups == 0)
    {
      /* Check 2: No extension groups, this is not an error case as
       * we check the parent later. */
      *out_extension_group = NULL;
      return TRUE;
    }

  g_set_error (error,
               G_IO_ERROR,
               G_IO_ERROR_FAILED,
               "Unable to resolve extension %s to a unique "
               "extension point in the parent app or runtime. Consider "
               "using the 'tag' key in ExtensionOf to disambiguate which "
               "extension point to build against.",
               id);

  return FALSE;
}