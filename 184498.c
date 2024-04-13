GetStringPlatform(JNIEnv* env, jstring instr, char* outstr, jint outlen, const char* encoding)
{
	jint result;

	preloadLibraries();

	Trc_SC_GetStringPlatform_Entry(env, instr, outstr, outlen, encoding);

	result = globalGetStringPlatform(env, instr, outstr, outlen, encoding);

	Trc_SC_GetStringPlatform_Exit(env, result);

	return result;
}