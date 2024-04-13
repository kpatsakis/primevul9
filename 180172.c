ves_icall_System_Runtime_CompilerServices_RuntimeHelpers_GetOffsetToStringData (void)
{
	MONO_ARCH_SAVE_REGS;

	return offsetof (MonoString, chars);
}