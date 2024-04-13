ves_icall_System_Buffer_ByteLengthInternal (MonoArray *array) 
{
	MONO_ARCH_SAVE_REGS;

	return mono_array_get_byte_length (array);
}