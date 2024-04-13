throwNewUnsatisfiedLinkError(JNIEnv *env, char *message) {
	jclass exceptionClass = (*env)->FindClass(env, "java/lang/UnsatisfiedLinkError");
	if (NULL != exceptionClass) {
		(*env)->ThrowNew(env, exceptionClass, message);
 	}
 	return;
}