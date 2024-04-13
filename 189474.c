file_recursive_delete_thread (GSimpleAsyncResult *simple,
                              GObject *object,
                              GCancellable *cancellable)
{
	GError *error = NULL;

	e_file_recursive_delete_sync (G_FILE (object), cancellable, &error);

	if (error != NULL)
		g_simple_async_result_take_error (simple, error);
}