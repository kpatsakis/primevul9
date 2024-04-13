void JNICALL JVM_MonitorNotify(JNIEnv *env, jobject anObject) {
	Trc_SC_MonitorNotify_Entry(env, anObject);

	(*env)->CallVoidMethod(env, anObject, notifyMID);

	Trc_SC_MonitorNotify_Exit(env);
}