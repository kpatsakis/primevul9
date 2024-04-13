ves_icall_get_attributes (MonoReflectionType *type)
{
	MonoClass *klass = mono_class_from_mono_type (type->type);

	MONO_ARCH_SAVE_REGS;

	return klass->flags;
}