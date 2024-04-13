ves_icall_MonoType_get_DeclaringType (MonoReflectionType *type)
{
	MonoDomain *domain = mono_domain_get ();
	MonoClass *class;

	MONO_ARCH_SAVE_REGS;

	if (type->type->byref)
		return NULL;
	if (type->type->type == MONO_TYPE_VAR)
		class = mono_type_get_generic_param_owner (type->type)->owner.klass;
	else if (type->type->type == MONO_TYPE_MVAR)
		class = mono_type_get_generic_param_owner (type->type)->owner.method->klass;
	else
		class = mono_class_from_mono_type (type->type)->nested_in;

	return class ? mono_type_get_object (domain, &class->byval_arg) : NULL;
}