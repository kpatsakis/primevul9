InternalFromBase64String (MonoString *str, MonoBoolean allowWhitespaceOnly)
{
	MONO_ARCH_SAVE_REGS;

	return base64_to_byte_array (mono_string_chars (str), 
		mono_string_length (str), allowWhitespaceOnly);
}