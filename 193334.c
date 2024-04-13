mono_thread_cleanup_apartment_state (void)
{
#ifdef HOST_WIN32
	MonoInternalThread* thread = mono_thread_internal_current ();

	if (thread && thread->apartment_state != ThreadApartmentState_Unknown) {
		CoUninitialize ();
	}
#endif
}