GetStringPlatformLength(JNIEnv* env, jstring instr, jint* outlen, const char* encoding)
{
	jint result;

	preloadLibraries();

	Trc_SC_GetStringPlatformLength_Entry(env, instr, outlen, encoding);
	result = globalGetStringPlatformLength(env, instr, outlen, encoding);
	Trc_SC_GetStringPlatformLength_Exit(env, result, *outlen);

	return result;
}