ves_icall_System_Runtime_CompilerServices_RuntimeHelpers_RunClassConstructor (MonoType *handle)
{
	MonoClass *klass;
	MonoVTable *vtable;

	MONO_CHECK_ARG_NULL (handle);

	klass = mono_class_from_mono_type (handle);
	MONO_CHECK_ARG (handle, klass);

	vtable = mono_class_vtable_full (mono_domain_get (), klass, TRUE);

	/* This will call the type constructor */
	mono_runtime_class_init (vtable);
}