JVM_ClassLoaderDepth(JNIEnv *env)
{
	jint result;

	Trc_SC_ClassLoaderDepth_Entry(env);

	result = (*env)->CallStaticIntMethod(env, jlClass, classLoaderDepthMID);

	/* CMVC 95169: ensure that the result is a well defined error value if an exception occurred */
	if ((*env)->ExceptionCheck(env)) {
		result = -1;
	}

	Trc_SC_ClassLoaderDepth_Exit(env, result);

	return result;
}