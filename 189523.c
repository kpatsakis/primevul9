e_async_closure_callback (GObject *object,
                          GAsyncResult *result,
                          gpointer closure)
{
	EAsyncClosure *real_closure;

	g_return_if_fail (G_IS_ASYNC_RESULT (result));
	g_return_if_fail (closure != NULL);

	real_closure = closure;

	g_mutex_lock (&real_closure->lock);

	/* Replace any previous result. */
	if (real_closure->result != NULL)
		g_object_unref (real_closure->result);
	real_closure->result = g_object_ref (result);
	real_closure->finished = TRUE;

	g_mutex_unlock (&real_closure->lock);

	g_main_loop_quit (real_closure->loop);
}