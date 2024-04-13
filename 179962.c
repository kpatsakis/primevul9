ves_icall_System_Diagnostics_Debugger_IsAttached_internal (void)
{
	return mono_debug_using_mono_debugger () || mono_is_debugger_attached ();
}