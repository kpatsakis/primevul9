void mono_thread_force_interruption_checkpoint ()
{
	mono_thread_interruption_checkpoint_request (TRUE);
}