create_internal_thread_object (void)
{
	MonoVTable *vt = mono_class_vtable (mono_get_root_domain (), mono_defaults.internal_thread_class);
	return (MonoInternalThread*)mono_gc_alloc_mature (vt);
}