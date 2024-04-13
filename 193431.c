mono_thread_destroy_domain_tls (MonoDomain *domain)
{
	while (domain->tlsrec_list)
		mono_thread_destroy_tls (domain->tlsrec_list->tls_offset);
}