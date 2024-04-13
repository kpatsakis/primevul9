free_thread_static_data_helper (gpointer key, gpointer value, gpointer user)
{
	MonoInternalThread *thread = value;
	TlsOffsetSize *data = user;
	int idx = (data->offset >> 24) - 1;
	char *ptr;

	if (!thread->static_data || !thread->static_data [idx])
		return;
	ptr = ((char*) thread->static_data [idx]) + (data->offset & 0xffffff);
	memset (ptr, 0, data->size);
}