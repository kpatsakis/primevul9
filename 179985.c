ves_icall_type_ispointer (MonoReflectionType *type)
{
	MONO_ARCH_SAVE_REGS;

	return type->type->type == MONO_TYPE_PTR;
}