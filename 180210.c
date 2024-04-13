ves_icall_System_Reflection_MethodBase_GetMethodFromHandleInternalType (MonoMethod *method, MonoType *type)
{
	MonoClass *klass;
	if (type) {
		klass = mono_class_from_mono_type (type);
		if (mono_class_get_generic_type_definition (method->klass) != mono_class_get_generic_type_definition (klass)) 
			return NULL;
		if (method->klass != klass) {
			method = mono_method_get_equivalent_method (method, klass);
			if (!method)
				return NULL;
		}
	} else
		klass = method->klass;
	return mono_method_get_object (mono_domain_get (), method, klass);
}