static int hidp_session_wake_function(wait_queue_entry_t *wait,
				      unsigned int mode,
				      int sync, void *key)
{
	wake_up_interruptible(&hidp_session_wq);
	return false;
}