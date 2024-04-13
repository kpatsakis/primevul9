getVersionFromReleaseFile(void)
{
	PORT_ACCESS_FROM_PORT(&j9portLibrary);
	J9StringBuffer *propsPathBuffer = NULL;
	j9props_file_t propsFile = NULL;
	UDATA finalVersion = 0;

	propsPathBuffer = jvmBufferCat(propsPathBuffer, jvmBufferData(j9Buffer));
	propsPathBuffer = jvmBufferCat(propsPathBuffer, DIR_SEPARATOR_STR "release");
	propsFile = props_file_open(PORTLIB, jvmBufferData(propsPathBuffer), NULL, 0);
	free(propsPathBuffer);
	propsPathBuffer = NULL;
	if (NULL != propsFile) {
		const char *version = props_file_get(propsFile, "JAVA_VERSION");
		if (NULL != version) {
#define	 JAVA_VERSION_8 "\"1.8.0" /* its usual format is "1.8.0_xxx" */
			if (!strncmp(version, JAVA_VERSION_8, sizeof(JAVA_VERSION_8) - 1)) {
#undef   JAVA_VERSION_8
				finalVersion = J2SE_18 | J2SE_SHAPE_OPENJDK;
#define	 JAVA_VERSION_9 "\"9" /* its usual format is "9[.x.x]" */
			} else if (!strncmp(version, JAVA_VERSION_9, sizeof(JAVA_VERSION_9) - 1)) {
#undef   JAVA_VERSION_9
				finalVersion = J2SE_19 | J2SE_SHAPE_B165;
#define	 JAVA_VERSION_10 "\"10" /* its usual format is "10[.x.x]" */
			} else if (!strncmp(version, JAVA_VERSION_10, sizeof(JAVA_VERSION_10) - 1)) {
#undef   JAVA_VERSION_10			
				finalVersion = J2SE_V10 | J2SE_SHAPE_V10;
#define	 JAVA_VERSION_11 "\"11" /* its usual format is "11[.x.x]" */
			} else if (!strncmp(version, JAVA_VERSION_11, sizeof(JAVA_VERSION_11) - 1)) {
#undef   JAVA_VERSION_11
				finalVersion = J2SE_V11 | J2SE_SHAPE_V11;
#define	 JAVA_VERSION_12 "\"12" /* its usual format is "12[.x.x]" */
			} else if (!strncmp(version, JAVA_VERSION_12, sizeof(JAVA_VERSION_12) - 1)) {
#undef   JAVA_VERSION_12
				finalVersion = J2SE_V12 | J2SE_SHAPE_V12;
			} else {
				/* Assume latest Java version and shape */
				finalVersion = J2SE_LATEST | J2SE_SHAPE_LATEST;
			}
		} else {
#ifdef DEBUG
			printf("No 'JAVA_VERSION' property in %s\n", propsFile);
#endif
		}
		props_file_close(propsFile);
	} else {
#ifdef DEBUG
		printf("Could not open %s\n", propsFile);
#endif
	}

	return finalVersion;
}