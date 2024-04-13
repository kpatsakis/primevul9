ves_icall_System_Threading_Thread_SetName_internal (MonoInternalThread *this_obj, MonoString *name)
{
	ensure_synch_cs_set (this_obj);
	
	EnterCriticalSection (this_obj->synch_cs);
	
	if (this_obj->name) {
		LeaveCriticalSection (this_obj->synch_cs);
		
		mono_raise_exception (mono_get_exception_invalid_operation ("Thread.Name can only be set once."));
		return;
	}
	if (name) {
		this_obj->name = g_new (gunichar2, mono_string_length (name));
		memcpy (this_obj->name, mono_string_chars (name), mono_string_length (name) * 2);
		this_obj->name_len = mono_string_length (name);
	}
	else
		this_obj->name = NULL;
	
	LeaveCriticalSection (this_obj->synch_cs);
	if (this_obj->name) {
		char *tname = mono_string_to_utf8 (name);
		mono_profiler_thread_name (this_obj->tid, tname);
		mono_free (tname);
	}
}