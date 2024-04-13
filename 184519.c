JVM_FindLibraryEntry(void* handle, const char *functionName)
{
	void* result;

	Trc_SC_FindLibraryEntry_Entry(handle, functionName);

#if defined(WIN32)
	result = GetProcAddress ((HINSTANCE)handle, (LPCSTR)functionName);
#elif defined(J9UNIX) || defined(J9ZOS390) /* defined(WIN32) */
	result = (void*)dlsym( (void*)handle, (char *)functionName );
#else /* defined(WIN32) */
#error "Please implemente jvm.c:JVM_FindLibraryEntry(void* handle, const char *functionName)"
#endif /* defined(WIN32) */

	Trc_SC_FindLibraryEntry_Exit(result);

	return result;
}