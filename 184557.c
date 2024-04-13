getVersionFromPropertiesFile(void)
{
	if (-1 == jvmSEVersion) {
		UDATA finalVersion = 0;

		finalVersion = getVersionFromClasslibPropertiesFile();
		if (0 == finalVersion) {
			finalVersion = getVersionFromReleaseFile();
			if (0 == finalVersion) {
				return J2SE_LATEST | J2SE_SHAPE_LATEST;
			}
		}
		jvmSEVersion = finalVersion;
	}
	return jvmSEVersion;
}