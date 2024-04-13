JVM_GetInterfaceVersion(void)
{
	jint result = 4;
	UDATA j2seVersion = getVersionFromPropertiesFile();

	if ((j2seVersion & J2SE_SERVICE_RELEASE_MASK) >= J2SE_19) {
		result = 5;
	}

	Trc_SC_GetInterfaceVersion_Entry();

	Trc_SC_GetInterfaceVersion_Exit(result);

	return result;
}