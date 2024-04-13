search_tls_slot_in_freelist (StaticDataInfo *static_data, guint32 size, guint32 align)
{
	MonoThreadDomainTls* prev = NULL;
	MonoThreadDomainTls* tmp = static_data->freelist;
	while (tmp) {
		if (tmp->size == size) {
			if (prev)
				prev->next = tmp->next;
			else
				static_data->freelist = tmp->next;
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}