ves_icall_System_Diagnostics_DefaultTraceListener_WriteWindowsDebugString (MonoString *message)
{
#if defined (PLATFORM_WIN32)
	OutputDebugString (mono_string_chars (message));
#else
	g_warning ("WriteWindowsDebugString called and PLATFORM_WIN32 not defined!\n");
#endif
}