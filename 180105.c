ves_icall_MonoMethod_get_IsGenericMethodDefinition (MonoReflectionMethod *method)
{
	MONO_ARCH_SAVE_REGS;

	return method->method->is_generic;
}