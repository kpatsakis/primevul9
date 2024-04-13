e_async_closure_unlock_mutex_cb (gpointer user_data)
{
	EAsyncClosure *closure = user_data;

	g_return_val_if_fail (closure != NULL, FALSE);

	g_mutex_unlock (&closure->lock);

	return FALSE;
}