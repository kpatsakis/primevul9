ves_icall_System_Threading_Thread_Yield (void)
{
#ifdef HOST_WIN32
	return SwitchToThread ();
#else
	return sched_yield () == 0;
#endif
}