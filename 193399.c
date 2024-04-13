mono_thread_create (MonoDomain *domain, gpointer func, gpointer arg)
{
	mono_thread_create_internal (domain, func, arg, FALSE, 0);
}