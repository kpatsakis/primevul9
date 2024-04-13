ves_icall_System_Environment_get_MachineName (void)
{
#if defined (PLATFORM_WIN32)
	gunichar2 *buf;
	guint32 len;
	MonoString *result;

	len = MAX_COMPUTERNAME_LENGTH + 1;
	buf = g_new (gunichar2, len);

	result = NULL;
	if (GetComputerName (buf, (PDWORD) &len))
		result = mono_string_new_utf16 (mono_domain_get (), buf, len);

	g_free (buf);
	return result;
#elif !defined(DISABLE_SOCKETS)
	gchar buf [256];
	MonoString *result;

	if (gethostname (buf, sizeof (buf)) == 0)
		result = mono_string_new (mono_domain_get (), buf);
	else
		result = NULL;
	
	return result;
#else
	return mono_string_new (mono_domain_get (), "mono");
#endif
}