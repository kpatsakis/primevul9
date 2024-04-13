ves_icall_System_Threading_Thread_GetName_internal (MonoInternalThread *this_obj)
{
	MonoString* str;

	ensure_synch_cs_set (this_obj);
	
	EnterCriticalSection (this_obj->synch_cs);
	
	if (!this_obj->name)
		str = NULL;
	else
		str = mono_string_new_utf16 (mono_domain_get (), this_obj->name, this_obj->name_len);
	
	LeaveCriticalSection (this_obj->synch_cs);
	
	return str;
}