_g_path_get_temp_work_dir (const char *parent_folder)
{
        guint64  max_size = 0;
        char    *best_folder = NULL;
        int      i;
        char    *template;
        char    *result = NULL;

        if (parent_folder == NULL) {
                /* find the folder with more free space. */

                for (i = 0; try_folder[i] != NULL; i++) {
                        const char *folder;
                        GFile      *file;
                        guint64     size;

                        folder = get_nth_temp_folder_to_try (i);
                        file = g_file_new_for_path (folder);
                        size = _g_file_get_free_space (file);
                        g_object_unref (file);

                        if (max_size < size) {
                                max_size = size;
                                g_free (best_folder);
                                best_folder = g_strdup (folder);
                        }
                }
        }
        else
                best_folder = g_strdup (parent_folder);

        if (best_folder == NULL)
                return NULL;

        template = g_strconcat (best_folder, "/.fr-XXXXXX", NULL);
        result = mkdtemp (template);
        g_free (best_folder);

        if ((result == NULL) || (*result == '\0')) {
                g_free (template);
                result = NULL;
        }

        return result;
}