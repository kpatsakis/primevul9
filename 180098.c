ves_icall_Type_get_IsGenericTypeDefinition (MonoReflectionType *type)
{
	MonoClass *klass;
	MONO_ARCH_SAVE_REGS;

	if (!IS_MONOTYPE (type))
		return FALSE;

	if (type->type->byref)
		return FALSE;

	klass = mono_class_from_mono_type (type->type);

	return klass->generic_container != NULL;
}