JVM_CurrentLoadedClass(JNIEnv *env)
{
	jobject result;

	Trc_SC_CurrentLoadedClass_Entry(env);

	result = (*env)->CallStaticObjectMethod(env, jlClass, currentLoadedClassMID);

	/* CMVC 95169: ensure that the result is a well defined error value if an exception occurred */
	if ((*env)->ExceptionCheck(env)) {
		result = NULL;
	}

	Trc_SC_CurrentLoadedClass_Exit(env, result);

	return result;
}