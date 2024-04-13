ves_icall_System_ComponentModel_Win32Exception_W32ErrorMessage (guint32 code)
{
	MonoString *message;
	guint32 ret;
	gunichar2 buf[256];
	
	ret = FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM |
			     FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, 0,
			     buf, 255, NULL);
	if (ret == 0) {
		message = mono_string_new (mono_domain_get (), "Error looking up error string");
	} else {
		message = mono_string_new_utf16 (mono_domain_get (), buf, ret);
	}
	
	return message;
}