mono_thread_alloc_tls (MonoReflectionType *type)
{
	MonoDomain *domain = mono_domain_get ();
	MonoClass *klass;
	MonoTlsDataRecord *tlsrec;
	int max_set = 0;
	gsize *bitmap;
	gsize default_bitmap [4] = {0};
	uint32_t tls_offset;
	guint32 size;
	gint32 align;

	klass = mono_class_from_mono_type (type->type);
	/* TlsDatum is a struct, so we subtract the object header size offset */
	bitmap = mono_class_compute_bitmap (klass, default_bitmap, sizeof (default_bitmap) * 8, - (int)(sizeof (MonoObject) / sizeof (gpointer)), &max_set, FALSE);
	size = mono_type_size (type->type, &align);
	tls_offset = mono_alloc_special_static_data (SPECIAL_STATIC_THREAD, size, align, bitmap, max_set);
	if (bitmap != default_bitmap)
		g_free (bitmap);
	tlsrec = g_new0 (MonoTlsDataRecord, 1);
	tlsrec->tls_offset = tls_offset;
	tlsrec->size = size;
	mono_domain_lock (domain);
	tlsrec->next = domain->tlsrec_list;
	domain->tlsrec_list = tlsrec;
	mono_domain_unlock (domain);
	return tls_offset;
}