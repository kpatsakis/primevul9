ves_icall_Type_get_IsGenericType (MonoReflectionType *type)
{
	MonoClass *klass;
	MONO_ARCH_SAVE_REGS;

	if (!IS_MONOTYPE (type))
		return FALSE;

	if (type->type->byref)
		return FALSE;

	klass = mono_class_from_mono_type (type->type);
	return klass->generic_class != NULL || klass->generic_container != NULL;
}