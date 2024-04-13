imapx_sync_free_user (GArray *user_set)
{
	gint i;

	if (user_set == NULL)
		return;

	for (i = 0; i < user_set->len; i++) {
		struct _imapx_flag_change *flag_change = &g_array_index (user_set, struct _imapx_flag_change, i);
		GPtrArray *infos = flag_change->infos;
		gint j;

		for (j = 0; j < infos->len; j++) {
			CamelMessageInfo *info = g_ptr_array_index (infos, j);
			g_clear_object (&info);
		}

		g_ptr_array_free (infos, TRUE);
		g_free (flag_change->name);
	}
	g_array_free (user_set, TRUE);
}