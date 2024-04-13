clear_local_slot (gpointer key, gpointer value, gpointer user_data)
{
	LocalSlotID *sid = user_data;
	MonoInternalThread *thread = (MonoInternalThread*)value;
	MonoArray *slots_array;
	/*
	 * the static field is stored at: ((char*) thread->static_data [idx]) + (offset & 0xffffff);
	 * it is for the right domain, so we need to check if it is allocated an initialized
	 * for the current thread.
	 */
	/*g_print ("handling thread %p\n", thread);*/
	if (!thread->static_data || !thread->static_data [sid->idx])
		return;
	slots_array = *(MonoArray **)(((char*) thread->static_data [sid->idx]) + (sid->offset & 0xffffff));
	if (!slots_array || sid->slot >= mono_array_length (slots_array))
		return;
	mono_array_set (slots_array, MonoObject*, sid->slot, NULL);
}