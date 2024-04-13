ves_icall_System_Environment_InternalSetEnvironmentVariable (MonoString *name, MonoString *value)
{
	MonoError error;

#ifdef PLATFORM_WIN32
	gunichar2 *utf16_name, *utf16_value;
#else
	gchar *utf8_name, *utf8_value;
#endif

	MONO_ARCH_SAVE_REGS;
	
#ifdef PLATFORM_WIN32
	utf16_name = mono_string_to_utf16 (name);
	if ((value == NULL) || (mono_string_length (value) == 0) || (mono_string_chars (value)[0] == 0)) {
		SetEnvironmentVariable (utf16_name, NULL);
		g_free (utf16_name);
		return;
	}

	utf16_value = mono_string_to_utf16 (value);

	SetEnvironmentVariable (utf16_name, utf16_value);

	g_free (utf16_name);
	g_free (utf16_value);
#else
	utf8_name = mono_string_to_utf8 (name);	/* FIXME: this should be ascii */

	if ((value == NULL) || (mono_string_length (value) == 0) || (mono_string_chars (value)[0] == 0)) {
		g_unsetenv (utf8_name);
		g_free (utf8_name);
		return;
	}

	utf8_value = mono_string_to_utf8_checked (value, &error);
	if (!mono_error_ok (&error)) {
		g_free (utf8_name);
		mono_error_raise_exception (&error);
	}
	g_setenv (utf8_name, utf8_value, TRUE);

	g_free (utf8_name);
	g_free (utf8_value);
#endif
}