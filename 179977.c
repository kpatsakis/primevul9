ves_icall_System_Environment_GetEnvironmentVariable (MonoString *name)
{
	const gchar *value;
	gchar *utf8_name;

	MONO_ARCH_SAVE_REGS;

	if (name == NULL)
		return NULL;

	utf8_name = mono_string_to_utf8 (name);	/* FIXME: this should be ascii */
	value = g_getenv (utf8_name);

	g_free (utf8_name);

	if (value == 0)
		return NULL;
	
	return mono_string_new (mono_domain_get (), value);
}