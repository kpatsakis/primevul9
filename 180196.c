ves_icall_System_Buffer_GetByteInternal (MonoArray *array, gint32 idx) 
{
	MONO_ARCH_SAVE_REGS;

	return mono_array_get (array, gint8, idx);
}