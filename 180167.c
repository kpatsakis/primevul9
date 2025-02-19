ves_icall_System_Runtime_InteropServices_Marshal_PrelinkAll (MonoReflectionType *type)
{
	MonoClass *klass = mono_class_from_mono_type (type->type);
	MonoMethod* m;
	gpointer iter = NULL;
	MONO_ARCH_SAVE_REGS;

	while ((m = mono_class_get_methods (klass, &iter)))
		prelink_method (m);
}