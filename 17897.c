void cleanup(void)
{
#if defined(JAS_USE_JAS_INIT)
	jas_cleanup();
#else
	jas_cleanup_thread();
	jas_cleanup_library();
#endif
}