ves_icall_System_Threading_Thread_ConstructInternalThread (MonoThread *this)
{
	MonoInternalThread *internal = create_internal_thread_object ();

	internal->state = ThreadState_Unstarted;
	internal->apartment_state = ThreadApartmentState_Unknown;

	InterlockedCompareExchangePointer ((gpointer)&this->internal_thread, internal, NULL);
}