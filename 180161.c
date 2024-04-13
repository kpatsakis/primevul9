ves_icall_System_Reflection_MethodBase_GetMethodBodyInternal (MonoMethod *method)
{
	return mono_method_body_get_object (mono_domain_get (), method);
}