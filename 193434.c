static void ensure_synch_cs_set (MonoInternalThread *thread)
{
	CRITICAL_SECTION *synch_cs;
	
	if (thread->synch_cs != NULL) {
		return;
	}
	
	synch_cs = g_new0 (CRITICAL_SECTION, 1);
	InitializeCriticalSection (synch_cs);
	
	if (InterlockedCompareExchangePointer ((gpointer *)&thread->synch_cs,
					       synch_cs, NULL) != NULL) {
		/* Another thread must have installed this CS */
		DeleteCriticalSection (synch_cs);
		g_free (synch_cs);
	}
}