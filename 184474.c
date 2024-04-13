JVM_CurrentTimeMillis(JNIEnv *env, jint unused1)
{
	jlong result;

	if (env != NULL) {
		PORT_ACCESS_FROM_ENV(env);

		Trc_SC_CurrentTimeMillis_Entry(env, unused1);

		result = (jlong) j9time_current_time_millis();

		Trc_SC_CurrentTimeMillis_Exit(env, result);
	} else {
		PORT_ACCESS_FROM_JAVAVM(BFUjavaVM);

		result = (jlong) j9time_current_time_millis();
	}

	return result;
}