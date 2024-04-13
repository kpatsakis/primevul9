ves_icall_get_type_parent (MonoReflectionType *type)
{
	MonoClass *class = mono_class_from_mono_type (type->type);

	MONO_ARCH_SAVE_REGS;

	return class->parent ? mono_type_get_object (mono_object_domain (type), &class->parent->byval_arg): NULL;
}