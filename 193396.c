get_thread_static_data (MonoInternalThread *thread, guint32 offset)
{
	int idx;
	g_assert ((offset & 0x80000000) == 0);
	offset &= 0x7fffffff;
	idx = (offset >> 24) - 1;
	return ((char*) thread->static_data [idx]) + (offset & 0xffffff);
}