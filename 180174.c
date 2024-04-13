ves_icall_Type_IsArrayImpl (MonoReflectionType *t)
{
	MonoType *type;
	MonoBoolean res;

	MONO_ARCH_SAVE_REGS;

	type = t->type;
	res = !type->byref && (type->type == MONO_TYPE_ARRAY || type->type == MONO_TYPE_SZARRAY);

	return res;
}