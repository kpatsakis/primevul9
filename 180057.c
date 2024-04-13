ves_icall_MonoType_get_Module (MonoReflectionType *type)
{
	MonoClass *class = mono_class_from_mono_type (type->type);

	MONO_ARCH_SAVE_REGS;

	return mono_module_get_object (mono_object_domain (type), class->image);
}