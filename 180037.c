InternalFromBase64CharArray (MonoArray *input, gint offset, gint length)
{
	MONO_ARCH_SAVE_REGS;

	return base64_to_byte_array (mono_array_addr (input, gunichar2, offset),
		length, FALSE);
}