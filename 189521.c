e_file_recursive_delete (GFile *file,
                         gint io_priority,
                         GCancellable *cancellable,
                         GAsyncReadyCallback callback,
                         gpointer user_data)
{
	GSimpleAsyncResult *simple;

	g_return_if_fail (G_IS_FILE (file));

	simple = g_simple_async_result_new (
		G_OBJECT (file), callback, user_data,
		e_file_recursive_delete);

	g_simple_async_result_set_check_cancellable (simple, cancellable);

	g_simple_async_result_run_in_thread (
		simple, file_recursive_delete_thread,
		io_priority, cancellable);

	g_object_unref (simple);
}