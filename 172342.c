idle_thread_data_free (gpointer ptr)
{
	IdleThreadData *itd = ptr;

	if (itd) {
		g_clear_object (&itd->is);
		g_clear_object (&itd->idle_cancellable);
		g_slice_free (IdleThreadData, itd);
	}
}