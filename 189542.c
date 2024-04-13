e_async_closure_wait (EAsyncClosure *closure)
{
	g_return_val_if_fail (closure != NULL, NULL);

	g_mutex_lock (&closure->lock);
	if (closure->finished) {
		g_mutex_unlock (&closure->lock);
	} else {
		GSource *idle_source;

		/* Unlock the closure->lock in the main loop, to ensure thread safety.
		   It should be processed before anything else, otherwise deadlock happens. */
		idle_source = g_idle_source_new ();
		g_source_set_callback (idle_source, e_async_closure_unlock_mutex_cb, closure, NULL);
		g_source_set_priority (idle_source, G_PRIORITY_HIGH * 2);
		g_source_attach (idle_source, closure->context);
		g_source_unref (idle_source);

		g_main_loop_run (closure->loop);
	}

	return closure->result;
}