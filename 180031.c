ves_icall_MonoMethod_get_IsGenericMethod (MonoReflectionMethod *method)
{
	MONO_ARCH_SAVE_REGS;

	return mono_method_signature (method->method)->generic_param_count != 0;
}