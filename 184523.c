JVM_RawMonitorCreate(void)
{
	omrthread_monitor_t newMonitor;

	Trc_SC_RawMonitorCreate_Entry();

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
	if(omrthread_monitor_init_with_name(&newMonitor, 0, "JVM_RawMonitor"))
#else
	if(f_monitorInit(&newMonitor, 0, "JVM_RawMonitor"))
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
	{
		Trc_SC_RawMonitorCreate_Error();
		printf("error initializing raw monitor\n");
		exit(1);
	}

	Trc_SC_RawMonitorCreate_Exit(newMonitor);
	return (void *) newMonitor;
}