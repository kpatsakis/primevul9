ves_icall_Type_make_byref_type (MonoReflectionType *type)
{
	MonoClass *klass;

	MONO_ARCH_SAVE_REGS;

	klass = mono_class_from_mono_type (type->type);
	check_for_invalid_type (klass);

	return mono_type_get_object (mono_object_domain (type), &klass->this_arg);
}