setNLSCatalog(struct J9PortLibrary* portLib, UDATA j2seVersion)
{
	J9StringBuffer *nlsSearchPathBuffer = NULL;
	const char *nlsSearchPaths = NULL;
	PORT_ACCESS_FROM_PORT(portLib);

	if ((j2seVersion & J2SE_SERVICE_RELEASE_MASK) >= J2SE_19) {
		/*
		 * j9libBuffer doesn't end in a slash, but j9nls_set_catalog ignores everything after
		 * the last slash. Append a slash to our local copy of j9libBuffer
		 */
		nlsSearchPathBuffer = jvmBufferCat(nlsSearchPathBuffer, jvmBufferData(j9libBuffer));
	} else {
		/*
		 * j9binBuffer doesn't end in a slash, but j9nls_set_catalog ignores everything after
		 * the last slash. Append a slash to our local copy of j9bin
		 */
		nlsSearchPathBuffer = jvmBufferCat(nlsSearchPathBuffer, jvmBufferData(j9binBuffer));
	}
	nlsSearchPathBuffer = jvmBufferCat(nlsSearchPathBuffer, DIR_SEPARATOR_STR);
	nlsSearchPaths = jvmBufferData(nlsSearchPathBuffer);

	j9nls_set_catalog(&nlsSearchPaths, 1, "java", "properties");
	free(nlsSearchPathBuffer);
	nlsSearchPathBuffer = NULL;
}