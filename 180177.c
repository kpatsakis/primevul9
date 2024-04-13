ves_icall_TypeBuilder_get_IsGenericParameter (MonoReflectionTypeBuilder *tb)
{
	MONO_ARCH_SAVE_REGS;
	return is_generic_parameter (tb->type.type);
}