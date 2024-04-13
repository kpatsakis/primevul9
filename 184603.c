void JNICALL JVM_MonitorWait(JNIEnv *env, jobject anObject, jlong timeout) {
	Trc_SC_MonitorWait_Entry(env, anObject, timeout);

	(*env)->CallVoidMethod(env, anObject, waitMID, timeout);

	Trc_SC_MonitorWait_Exit(env);
}