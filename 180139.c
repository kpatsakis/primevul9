ves_icall_System_Reflection_MethodBase_GetMethodFromHandleInternal (MonoMethod *method)
{
	return mono_method_get_object (mono_domain_get (), method, NULL);
}