NewStringPlatform(JNIEnv* env, const char* instr, jstring* outstr, const char* encoding)
{
	jint result;

	preloadLibraries();
	Trc_SC_NewStringPlatform_Entry(env, instr, outstr, encoding);
	result = globalNewStringPlatform(env, instr, outstr, encoding);
	Trc_SC_NewStringPlatform_Exit(env, result);

	return result;
}