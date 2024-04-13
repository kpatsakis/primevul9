flatpak_run_add_extension_args (FlatpakBwrap      *bwrap,
                                GKeyFile          *metakey,
                                FlatpakDecomposed *ref,
                                gboolean           use_ld_so_cache,
                                const char        *target_path,
                                char             **extensions_out,
                                char             **ld_path_out,
                                GCancellable      *cancellable,
                                GError           **error)
{
  g_autoptr(GString) used_extensions = g_string_new ("");
  GList *extensions, *path_sorted_extensions, *l;
  g_autoptr(GString) ld_library_path = g_string_new ("");
  int count = 0;
  g_autoptr(GHashTable) mounted_tmpfs =
    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  g_autoptr(GHashTable) created_symlink =
    g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
  g_autofree char *arch = flatpak_decomposed_dup_arch (ref);
  const char *branch = flatpak_decomposed_get_branch (ref);

  g_return_val_if_fail (target_path != NULL, FALSE);

  extensions = flatpak_list_extensions (metakey, arch, branch);

  /* First we apply all the bindings, they are sorted alphabetically in order for parent directory
     to be mounted before child directories */
  path_sorted_extensions = g_list_copy (extensions);
  path_sorted_extensions = g_list_sort (path_sorted_extensions, flatpak_extension_compare_by_path);

  for (l = path_sorted_extensions; l != NULL; l = l->next)
    {
      FlatpakExtension *ext = l->data;
      g_autofree char *directory = g_build_filename (target_path, ext->directory, NULL);
      g_autofree char *full_directory = g_build_filename (directory, ext->subdir_suffix, NULL);
      g_autofree char *ref_file = g_build_filename (full_directory, ".ref", NULL);
      g_autofree char *real_ref = g_build_filename (ext->files_path, ext->directory, ".ref", NULL);

      if (ext->needs_tmpfs)
        {
          g_autofree char *parent = g_path_get_dirname (directory);

          if (g_hash_table_lookup (mounted_tmpfs, parent) == NULL)
            {
              flatpak_bwrap_add_args (bwrap,
                                      "--tmpfs", parent,
                                      NULL);
              g_hash_table_insert (mounted_tmpfs, g_steal_pointer (&parent), "mounted");
            }
        }

      flatpak_bwrap_add_args (bwrap,
                              "--ro-bind", ext->files_path, full_directory,
                              NULL);

      if (g_file_test (real_ref, G_FILE_TEST_EXISTS))
        flatpak_bwrap_add_args (bwrap,
                                "--lock-file", ref_file,
                                NULL);
    }

  g_list_free (path_sorted_extensions);

  /* Then apply library directories and file merging, in extension prio order */

  for (l = extensions; l != NULL; l = l->next)
    {
      FlatpakExtension *ext = l->data;
      g_autofree char *directory = g_build_filename (target_path, ext->directory, NULL);
      g_autofree char *full_directory = g_build_filename (directory, ext->subdir_suffix, NULL);
      int i;

      if (used_extensions->len > 0)
        g_string_append (used_extensions, ";");
      g_string_append (used_extensions, ext->installed_id);
      g_string_append (used_extensions, "=");
      if (ext->commit != NULL)
        g_string_append (used_extensions, ext->commit);
      else
        g_string_append (used_extensions, "local");

      if (ext->add_ld_path)
        {
          g_autofree char *ld_path = g_build_filename (full_directory, ext->add_ld_path, NULL);

          if (use_ld_so_cache)
            {
              g_autofree char *contents = g_strconcat (ld_path, "\n", NULL);
              /* We prepend app or runtime and a counter in order to get the include order correct for the conf files */
              g_autofree char *ld_so_conf_file = g_strdup_printf ("%s-%03d-%s.conf", flatpak_decomposed_get_kind_str (ref), ++count, ext->installed_id);
              g_autofree char *ld_so_conf_file_path = g_build_filename ("/run/flatpak/ld.so.conf.d", ld_so_conf_file, NULL);

              if (!flatpak_bwrap_add_args_data (bwrap, "ld-so-conf",
                                                contents, -1, ld_so_conf_file_path, error))
                return FALSE;
            }
          else
            {
              if (ld_library_path->len != 0)
                g_string_append (ld_library_path, ":");
              g_string_append (ld_library_path, ld_path);
            }
        }

      for (i = 0; ext->merge_dirs != NULL && ext->merge_dirs[i] != NULL; i++)
        {
          g_autofree char *parent = g_path_get_dirname (directory);
          g_autofree char *merge_dir = g_build_filename (parent, ext->merge_dirs[i], NULL);
          g_autofree char *source_dir = g_build_filename (ext->files_path, ext->merge_dirs[i], NULL);
          g_auto(GLnxDirFdIterator) source_iter = { 0 };
          struct dirent *dent;

          if (glnx_dirfd_iterator_init_at (AT_FDCWD, source_dir, TRUE, &source_iter, NULL))
            {
              while (glnx_dirfd_iterator_next_dent (&source_iter, &dent, NULL, NULL) && dent != NULL)
                {
                  g_autofree char *symlink_path = g_build_filename (merge_dir, dent->d_name, NULL);
                  /* Only create the first, because extensions are listed in prio order */
                  if (g_hash_table_lookup (created_symlink, symlink_path) == NULL)
                    {
                      g_autofree char *symlink = g_build_filename (directory, ext->merge_dirs[i], dent->d_name, NULL);
                      flatpak_bwrap_add_args (bwrap,
                                              "--symlink", symlink, symlink_path,
                                              NULL);
                      g_hash_table_insert (created_symlink, g_steal_pointer (&symlink_path), "created");
                    }
                }
            }
        }
    }

  g_list_free_full (extensions, (GDestroyNotify) flatpak_extension_free);

  if (extensions_out)
    *extensions_out = g_string_free (g_steal_pointer (&used_extensions), FALSE);

  if (ld_path_out)
    *ld_path_out = g_string_free (g_steal_pointer (&ld_library_path), FALSE);

  return TRUE;
}