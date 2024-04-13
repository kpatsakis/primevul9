JVM_RawMonitorDestroy(void* mon)
{

	Trc_SC_RawMonitorDestroy_Entry(mon);

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	omrthread_monitor_destroy((omrthread_monitor_t) mon);
#else
	f_monitorDestroy((omrthread_monitor_t) mon);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

	Trc_SC_RawMonitorDestroy_Exit();
}