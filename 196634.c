is_valid_generic_instantiation (MonoGenericContainer *gc, MonoGenericContext *context, MonoGenericInst *ginst)
{
	MonoError error;
	int i;

	if (ginst->type_argc != gc->type_argc)
		return FALSE;

	for (i = 0; i < gc->type_argc; ++i) {
		MonoGenericParamInfo *param_info = mono_generic_container_get_param_info (gc, i);
		MonoClass *paramClass;
		MonoClass **constraints;

		if (!param_info->constraints && !(param_info->flags & GENERIC_PARAMETER_ATTRIBUTE_SPECIAL_CONSTRAINTS_MASK))
			continue;
		if (mono_type_is_generic_argument (ginst->type_argv [i]))
			continue; //it's not our job to validate type variables

		paramClass = mono_class_from_mono_type (ginst->type_argv [i]);

		if (paramClass->exception_type != MONO_EXCEPTION_NONE)
			return FALSE;

		/*it's not safe to call mono_class_init from here*/
		if (paramClass->generic_class && !paramClass->inited) {
			if (!mono_class_is_valid_generic_instantiation (NULL, paramClass))
				return FALSE;
		}

		if ((param_info->flags & GENERIC_PARAMETER_ATTRIBUTE_VALUE_TYPE_CONSTRAINT) && (!paramClass->valuetype || mono_class_is_nullable (paramClass)))
			return FALSE;

		if ((param_info->flags & GENERIC_PARAMETER_ATTRIBUTE_REFERENCE_TYPE_CONSTRAINT) && paramClass->valuetype)
			return FALSE;

		if ((param_info->flags & GENERIC_PARAMETER_ATTRIBUTE_CONSTRUCTOR_CONSTRAINT) && !paramClass->valuetype && !mono_class_has_default_constructor (paramClass))
			return FALSE;

		if (!param_info->constraints)
			continue;

		for (constraints = param_info->constraints; *constraints; ++constraints) {
			MonoClass *ctr = *constraints;
			MonoType *inflated;

			inflated = mono_class_inflate_generic_type_checked (&ctr->byval_arg, context, &error);
			if (!mono_error_ok (&error)) {
				mono_error_cleanup (&error);
				return FALSE;
			}
			ctr = mono_class_from_mono_type (inflated);
			mono_metadata_free_type (inflated);

			if (!mono_class_is_constraint_compatible (paramClass, ctr))
				return FALSE;
		}
	}
	return TRUE;
}