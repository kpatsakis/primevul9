imapx_server_inactivity_timeout_cb (gpointer data)
{
	CamelIMAPXServer *is;
	GThread *thread;
	GError *local_error = NULL;

	is = g_weak_ref_get (data);

	if (is == NULL)
		return G_SOURCE_REMOVE;

	thread = g_thread_try_new (NULL, imapx_server_inactivity_thread, g_object_ref (is), &local_error);
	if (!thread) {
		g_warning ("%s: Failed to start inactivity thread: %s", G_STRFUNC, local_error ? local_error->message : "Unknown error");
		g_object_unref (is);
	} else {
		g_thread_unref (thread);
	}

	g_clear_error (&local_error);
	g_object_unref (is);

	return G_SOURCE_REMOVE;
}