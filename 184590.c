JNI_a2e_vsprintf(char *target, const char *format, va_list args)
{
	jint result;

	preloadLibraries();

	Trc_SC_a2e_vsprintf_Entry(target, format);

	result = global_a2e_vsprintf(target, format, args);

	Trc_SC_a2e_vsprintf_Exit(result);

	return result;
}