void mono_thread_interruption_checkpoint ()
{
	mono_thread_interruption_checkpoint_request (FALSE);
}