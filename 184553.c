JVM_Sync(jint descriptor)
{
	jint result;

	Trc_SC_Sync_Entry(descriptor);

	if (descriptor == -1) {
		Trc_SC_Sync_bad_descriptor();
		return -1;
	}

#if defined(WIN32)
#ifdef WIN32_IBMC
	printf("_JVM_Sync@4 called but not yet implemented. Exiting.\n");
	exit(44);
#else
	result = _commit(descriptor);
#endif
#elif defined(J9UNIX) || defined(J9ZOS390) /* defined(WIN32) */
	result = fsync(descriptor);
#else /* defined(WIN32) */
#error No JVM_Sync implementation
#endif /* defined(WIN32) */

	Trc_SC_Sync_Exit(result);

	return result;
}