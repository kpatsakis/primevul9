ves_icall_type_isprimitive (MonoReflectionType *type)
{
	MONO_ARCH_SAVE_REGS;

	return (!type->type->byref && (((type->type->type >= MONO_TYPE_BOOLEAN) && (type->type->type <= MONO_TYPE_R8)) || (type->type->type == MONO_TYPE_I) || (type->type->type == MONO_TYPE_U)));
}