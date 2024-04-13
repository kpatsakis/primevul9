new_thread_with_internal (MonoDomain *domain, MonoInternalThread *internal)
{
	MonoThread *thread = create_thread_object (domain);
	MONO_OBJECT_SETREF (thread, internal_thread, internal);
	return thread;
}