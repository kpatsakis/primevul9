ves_icall_MonoType_get_IsGenericParameter (MonoReflectionType *type)
{
	MONO_ARCH_SAVE_REGS;
	return is_generic_parameter (type->type);
}