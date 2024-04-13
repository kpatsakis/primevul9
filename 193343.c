mono_get_special_static_data_for_thread (MonoInternalThread *thread, guint32 offset)
{
	/* The high bit means either thread (0) or static (1) data. */

	guint32 static_type = (offset & 0x80000000);
	int idx;

	offset &= 0x7fffffff;
	idx = (offset >> 24) - 1;

	if (static_type == 0) {
		return get_thread_static_data (thread, offset);
	} else {
		/* Allocate static data block under demand, since we don't have a list
		// of contexts
		*/
		MonoAppContext *context = mono_context_get ();
		if (!context->static_data || !context->static_data [idx]) {
			mono_contexts_lock ();
			mono_alloc_static_data (&(context->static_data), offset, FALSE);
			mono_contexts_unlock ();
		}
		return ((char*) context->static_data [idx]) + (offset & 0xffffff);	
	}
}