ves_icall_Type_GetGenericParameterAttributes (MonoReflectionType *type)
{
	MONO_ARCH_SAVE_REGS;

	g_assert (IS_MONOTYPE (type));
	g_assert (is_generic_parameter (type->type));
	return mono_generic_param_info (type->type->data.generic_param)->flags;
}