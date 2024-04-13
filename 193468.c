static void terminate_thread (gpointer key, gpointer value, gpointer user)
{
	MonoInternalThread *thread=(MonoInternalThread *)value;
	
	if(thread->tid != (gsize)user) {
		/*TerminateThread (thread->handle, -1);*/
	}
}