e_util_free_nullable_object_slist (GSList *objects)
{
	const GSList *l;
	for (l = objects; l; l = l->next) {
		if (l->data)
			g_object_unref (l->data);
	}
	g_slist_free (objects);
}