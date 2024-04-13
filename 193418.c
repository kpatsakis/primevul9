mono_thread_get_name (MonoInternalThread *this_obj, guint32 *name_len)
{
	gunichar2 *res;

	ensure_synch_cs_set (this_obj);
	
	EnterCriticalSection (this_obj->synch_cs);
	
	if (!this_obj->name) {
		*name_len = 0;
		res = NULL;
	} else {
		*name_len = this_obj->name_len;
		res = g_new (gunichar2, this_obj->name_len);
		memcpy (res, this_obj->name, sizeof (gunichar2) * this_obj->name_len);
	}
	
	LeaveCriticalSection (this_obj->synch_cs);

	return res;
}