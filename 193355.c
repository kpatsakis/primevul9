gint32* mono_thread_interruption_request_flag ()
{
	return &thread_interruption_requested;
}