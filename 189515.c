e_async_closure_new (void)
{
	EAsyncClosure *closure;

	closure = g_slice_new0 (EAsyncClosure);
	closure->context = g_main_context_new ();
	closure->loop = g_main_loop_new (closure->context, FALSE);
	closure->finished = FALSE;
	g_mutex_init (&closure->lock);

	g_main_context_push_thread_default (closure->context);

	return closure;
}