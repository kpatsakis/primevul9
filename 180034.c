ves_icall_Type_GetGenericParameterConstraints (MonoReflectionType *type)
{
	MonoGenericParamInfo *param_info;
	MonoDomain *domain;
	MonoClass **ptr;
	MonoArray *res;
	int i, count;

	MONO_ARCH_SAVE_REGS;

	g_assert (IS_MONOTYPE (type));

	domain = mono_object_domain (type);
	param_info = mono_generic_param_info (type->type->data.generic_param);
	for (count = 0, ptr = param_info->constraints; ptr && *ptr; ptr++, count++)
		;

	res = mono_array_new (domain, mono_defaults.monotype_class, count);
	for (i = 0; i < count; i++)
		mono_array_setref (res, i, mono_type_get_object (domain, &param_info->constraints [i]->byval_arg));


	return res;
}