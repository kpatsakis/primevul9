ves_icall_RuntimeMethod_GetFunctionPointer (MonoMethod *method)
{
	MONO_ARCH_SAVE_REGS;

	return mono_compile_method (method);
}