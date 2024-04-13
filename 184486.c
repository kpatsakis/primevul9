getVersionFromClasslibPropertiesFile(void)
{
	PORT_ACCESS_FROM_PORT(&j9portLibrary);
	J9StringBuffer *propsPathBuffer = NULL;
	j9props_file_t propsFile = NULL;
	UDATA finalVersion = 0;

	propsPathBuffer = jvmBufferCat(propsPathBuffer, jvmBufferData(j9libBuffer));
	propsPathBuffer = jvmBufferCat(propsPathBuffer, DIR_SEPARATOR_STR "classlib.properties");
	propsFile = props_file_open(PORTLIB, jvmBufferData(propsPathBuffer), NULL, 0);
	free(propsPathBuffer);
	propsPathBuffer = NULL;

	if (NULL != propsFile) {
		const char *shape = NULL;
		const char *version = NULL;
		UDATA decoded = 0;
		
		shape = props_file_get(propsFile, "shape");
		if (NULL == shape) {
#ifdef DEBUG
			printf("No 'shape' property in %s\n", jvmBufferData(propsPathBuffer));
#endif
			goto bail;
		}

		version = props_file_get(propsFile, "version");
		if (NULL == version) {
#ifdef DEBUG
			printf("No 'version' property in %s\n", jvmBufferData(propsPathBuffer));
#endif
			goto bail;
		}
	
		decoded = decodeSetting("shape", shape, SHAPE_SETTINGS, NUM_SHAPE_SETTINGS);
		if (0 == decoded) {
			goto bail;
		}
		finalVersion |= decoded;
	
		decoded = decodeSetting("version", version, VERSION_SETTINGS, NUM_VERSION_SETTINGS);
		if (0 == decoded) {
			goto bail;
		}
		finalVersion |= decoded;

bail:
		props_file_close(propsFile);
	} else {
#ifdef DEBUG
		printf("Could not open %s\n", jvmBufferData(propsPathBuffer));
#endif
	}
	
	return finalVersion;
}