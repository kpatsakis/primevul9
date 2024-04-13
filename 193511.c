void mono_threads_install_notify_pending_exc (MonoThreadNotifyPendingExcFunc func)
{
	mono_thread_notify_pending_exc_fn = func;
}