ves_icall_MonoType_get_DeclaringMethod (MonoReflectionType *ref_type)
{
	MonoMethod *method;
	MonoType *type = ref_type->type;

	MONO_ARCH_SAVE_REGS;

	if (type->byref || (type->type != MONO_TYPE_MVAR && type->type != MONO_TYPE_VAR))
		mono_raise_exception (mono_get_exception_invalid_operation ("DeclaringMethod can only be used on generic arguments"));
	if (type->type == MONO_TYPE_VAR)
		return NULL;

	method = mono_type_get_generic_param_owner (type)->owner.method;
	g_assert (method);
	return mono_method_get_object (mono_object_domain (ref_type), method, method->klass);
}