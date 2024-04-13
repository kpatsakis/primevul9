ves_icall_System_Threading_Thread_MemoryBarrier (void)
{
	mono_threads_lock ();
	mono_threads_unlock ();
}