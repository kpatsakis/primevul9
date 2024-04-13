ves_icall_Type_MakePointerType (MonoReflectionType *type)
{
	MonoClass *klass, *pklass;


	MONO_ARCH_SAVE_REGS;

	klass = mono_class_from_mono_type (type->type);
	check_for_invalid_type (klass);

	pklass = mono_ptr_class_get (type->type);

	return mono_type_get_object (mono_object_domain (type), &pklass->byval_arg);
}