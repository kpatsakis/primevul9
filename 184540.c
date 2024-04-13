JVM_RawMonitorEnter(void* mon)
{
	Trc_SC_RawMonitorEnter_Entry(mon);

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	omrthread_monitor_enter((omrthread_monitor_t)mon);
#else
	f_monitorEnter((omrthread_monitor_t)mon);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

	Trc_SC_RawMonitorEnter_Exit();

	return 0;
}