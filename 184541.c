JVM_ClassDepth(JNIEnv *env, jstring name)
{
	jint result;

	Trc_SC_ClassDepth_Entry(env, name);

	result = (*env)->CallStaticIntMethod(env, jlClass, classDepthMID, name);

	/* CMVC 95169: ensure that the result is a well defined error value if an exception occurred */
	if ((*env)->ExceptionCheck(env)) {
		result = -1;
	}

	Trc_SC_ClassDepth_Exit(env, result);

	return result;
}