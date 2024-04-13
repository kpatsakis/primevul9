mono_thread_init_apartment_state (void)
{
#ifdef HOST_WIN32
	MonoInternalThread* thread = mono_thread_internal_current ();

	/* Positive return value indicates success, either
	 * S_OK if this is first CoInitialize call, or
	 * S_FALSE if CoInitialize already called, but with same
	 * threading model. A negative value indicates failure,
	 * probably due to trying to change the threading model.
	 */
	if (CoInitializeEx(NULL, (thread->apartment_state == ThreadApartmentState_STA) 
			? COINIT_APARTMENTTHREADED 
			: COINIT_MULTITHREADED) < 0) {
		thread->apartment_state = ThreadApartmentState_Unknown;
	}
#endif
}