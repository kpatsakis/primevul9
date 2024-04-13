e_util_unref_in_thread (gpointer object)
{
	GThread *thread;
	GError *error = NULL;

	if (!object)
		return;

	g_return_if_fail (G_IS_OBJECT (object));

	thread = g_thread_try_new (NULL, unref_object_in_thread, object, &error);
	if (thread) {
		g_thread_unref (thread);
	} else {
		g_warning ("%s: Failed to run thread: %s", G_STRFUNC, error ? error->message : "Unknown error");
		g_object_unref (object);
	}

	g_clear_error (&error);
}