ves_icall_System_Type_EqualsInternal (MonoReflectionType *type, MonoReflectionType *c)
{
	MONO_ARCH_SAVE_REGS;

	if (c && type->type && c->type)
		return mono_metadata_type_equal (type->type, c->type);
	else
		return (type == c) ? TRUE : FALSE;
}