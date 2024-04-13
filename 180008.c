ves_icall_System_Buffer_SetByteInternal (MonoArray *array, gint32 idx, gint8 value) 
{
	MONO_ARCH_SAVE_REGS;

	mono_array_set (array, gint8, idx, value);
}