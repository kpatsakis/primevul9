gpointer mono_create_thread (WapiSecurityAttributes *security,
							 guint32 stacksize, WapiThreadStart start,
							 gpointer param, guint32 create, gsize *tid)
{
	gpointer res;

#ifdef HOST_WIN32
	DWORD real_tid;

	res = CreateThread (security, stacksize, start, param, create, &real_tid);
	if (tid)
		*tid = real_tid;
#else
	res = CreateThread (security, stacksize, start, param, create, tid);
#endif

	return res;
}