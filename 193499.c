ves_icall_System_Threading_Thread_SetState (MonoInternalThread* this, guint32 state)
{
	mono_thread_set_state (this, state);
	
	if (state & ThreadState_Background) {
		/* If the thread changes the background mode, the main thread has to
		 * be notified, since it has to rebuild the list of threads to
		 * wait for.
		 */
		SetEvent (background_change_event);
	}
}