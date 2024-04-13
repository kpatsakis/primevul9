mono_threads_clear_cached_culture (MonoDomain *domain)
{
	mono_threads_lock ();
	mono_g_hash_table_foreach (threads, clear_cached_culture, domain);
	mono_threads_unlock ();
}