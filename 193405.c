do_free_special_slot (guint32 offset, guint32 size)
{
	guint32 static_type = (offset & 0x80000000);
	/*g_print ("free %s , size: %d, offset: %x\n", field->name, size, offset);*/
	if (static_type == 0) {
		TlsOffsetSize data;
		MonoThreadDomainTls *item = g_new0 (MonoThreadDomainTls, 1);
		data.offset = offset & 0x7fffffff;
		data.size = size;
		clear_reference_bitmap (data.offset, data.size);
		if (threads != NULL)
			mono_g_hash_table_foreach (threads, free_thread_static_data_helper, &data);
		item->offset = offset;
		item->size = size;

		if (!mono_runtime_is_shutting_down ()) {
			item->next = thread_static_info.freelist;
			thread_static_info.freelist = item;
		} else {
			/* We could be called during shutdown after mono_thread_cleanup () is called */
			g_free (item);
		}
	} else {
		/* FIXME: free context static data as well */
	}
}