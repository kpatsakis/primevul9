ves_icall_System_Environment_get_Platform (void)
{
#if defined (PLATFORM_WIN32)
	/* Win32NT */
	return 2;
#elif defined(__MACH__)
	/* OSX */
	if (mono_framework_version () < 2)
		return 128;

	//
	// For compatibility with our client code, this will be 4 for a while.
	// We will eventually move to 6 to match .NET, but it requires all client
	// code to be updated and the documentation everywhere to be updated 
	// first.
	//
	return 4;
#else
	/* Unix */
	if (mono_framework_version () < 2)
		return 128;
	return 4;
#endif
}