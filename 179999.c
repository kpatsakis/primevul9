ves_icall_MonoMethod_GetGenericArguments (MonoReflectionMethod *method)
{
	MonoArray *res;
	MonoDomain *domain;
	int count, i;
	MONO_ARCH_SAVE_REGS;

	domain = mono_object_domain (method);

	if (method->method->is_inflated) {
		MonoGenericInst *inst = mono_method_get_context (method->method)->method_inst;

		if (inst) {
			count = inst->type_argc;
			res = mono_array_new (domain, mono_defaults.systemtype_class, count);

			for (i = 0; i < count; i++)
				mono_array_setref (res, i, mono_type_get_object (domain, inst->type_argv [i]));

			return res;
		}
	}

	count = mono_method_signature (method->method)->generic_param_count;
	res = mono_array_new (domain, mono_defaults.systemtype_class, count);

	for (i = 0; i < count; i++) {
		MonoGenericContainer *container = mono_method_get_generic_container (method->method);
		MonoGenericParam *param = mono_generic_container_get_param (container, i);
		MonoClass *pklass = mono_class_from_generic_parameter (
			param, method->method->klass->image, TRUE);
		mono_array_setref (res, i,
				mono_type_get_object (domain, &pklass->byval_arg));
	}

	return res;
}