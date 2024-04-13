e_filename_mkdir_encoded (const gchar *basepath,
                          const gchar *fileprefix,
                          const gchar *filename,
                          gint fileindex)
{
	gchar *elem1, *elem2, *res, *fn;

	g_return_val_if_fail (basepath != NULL, NULL);
	g_return_val_if_fail (*basepath != 0, NULL);
	g_return_val_if_fail (fileprefix != NULL, NULL);
	g_return_val_if_fail (*fileprefix != 0, NULL);
	g_return_val_if_fail (!filename || *filename, NULL);

	if (g_mkdir_with_parents (basepath, 0700) < 0)
		return NULL;

	elem1 = g_strdup (fileprefix);
	if (filename)
		elem2 = g_strdup (filename);
	else
		elem2 = g_strdup_printf ("file%d", fileindex);

	e_filename_make_safe (elem1);
	e_filename_make_safe (elem2);

	fn = g_strconcat (elem1, "-", elem2, NULL);

	res = g_build_filename (basepath, fn, NULL);

	g_free (fn);
	g_free (elem1);
	g_free (elem2);

	return res;
}