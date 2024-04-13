mono_thread_get_tls_offset (void)
{
	int offset;
	MONO_THREAD_VAR_OFFSET (tls_current_object,offset);
	return offset;
}