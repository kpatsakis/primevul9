void JNICALL JVM_RawMonitorExit(void* mon) {

	Trc_SC_RawMonitorExit_Entry(mon);

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	omrthread_monitor_exit((omrthread_monitor_t)mon);
#else
	f_monitorExit((omrthread_monitor_t)mon);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

	Trc_SC_RawMonitorExit_Exit();
}