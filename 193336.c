static guint32 WINAPI start_wrapper(void *data)
{
#ifdef HAVE_SGEN_GC
	volatile int dummy;

	/* Avoid scanning the frames above this frame during a GC */
	mono_gc_set_stack_end ((void*)&dummy);
#endif

	return start_wrapper_internal (data);
}