ves_icall_System_Runtime_InteropServices_Marshal_Prelink (MonoReflectionMethod *method)
{
	MONO_ARCH_SAVE_REGS;
	prelink_method (method->method);
}