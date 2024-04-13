mono_thread_destroy_tls (uint32_t tls_offset)
{
	MonoTlsDataRecord *prev = NULL;
	MonoTlsDataRecord *cur;
	guint32 size = 0;
	MonoDomain *domain = mono_domain_get ();
	mono_domain_lock (domain);
	cur = domain->tlsrec_list;
	while (cur) {
		if (cur->tls_offset == tls_offset) {
			if (prev)
				prev->next = cur->next;
			else
				domain->tlsrec_list = cur->next;
			size = cur->size;
			g_free (cur);
			break;
		}
		prev = cur;
		cur = cur->next;
	}
	mono_domain_unlock (domain);
	if (size)
		mono_special_static_data_free_slot (tls_offset, size);
}