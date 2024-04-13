mono_thread_internal_current (void)
{
	MonoInternalThread *res = GET_CURRENT_OBJECT ();
	THREAD_DEBUG (g_message ("%s: returning %p", __func__, res));
	return res;
}