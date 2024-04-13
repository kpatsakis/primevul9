create_thread_object (MonoDomain *domain)
{
	MonoVTable *vt = mono_class_vtable (domain, mono_defaults.thread_class);
	return (MonoThread*)mono_gc_alloc_mature (vt);
}