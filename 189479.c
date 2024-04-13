e_async_closure_free (EAsyncClosure *closure)
{
	g_return_if_fail (closure != NULL);

	g_main_context_pop_thread_default (closure->context);

	g_main_loop_unref (closure->loop);
	g_main_context_unref (closure->context);

	g_mutex_lock (&closure->lock);
	g_clear_object (&closure->result);
	g_mutex_unlock (&closure->lock);
	g_mutex_clear (&closure->lock);

	g_slice_free (EAsyncClosure, closure);
}