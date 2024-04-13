ves_icall_type_isbyref (MonoReflectionType *type)
{
	MONO_ARCH_SAVE_REGS;

	return type->type->byref;
}