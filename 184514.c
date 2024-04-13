JVM_CurrentClassLoader(JNIEnv *env)
{
	jobject result;

	Trc_SC_CurrentClassLoader_Entry(env);

	result = (*env)->CallStaticObjectMethod(env, jlClass, currentClassLoaderMID);

	/* CMVC 95169: ensure that the result is a well defined error value if an exception occurred */
	if ((*env)->ExceptionCheck(env)) {
		result = NULL;
	}

	Trc_SC_CurrentClassLoader_Exit(env, result);

	return result;
}