ves_icall_Type_GetGenericParameterPosition (MonoReflectionType *type)
{
	MONO_ARCH_SAVE_REGS;

	if (!IS_MONOTYPE (type))
		return -1;

	if (is_generic_parameter (type->type))
		return mono_type_get_generic_param_num (type->type);
	return -1;
}