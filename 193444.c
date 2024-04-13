mono_special_static_data_free_slot (guint32 offset, guint32 size)
{
	mono_threads_lock ();
	do_free_special_slot (offset, size);
	mono_threads_unlock ();
}