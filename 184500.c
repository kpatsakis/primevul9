void JNICALL JVM_MonitorNotifyAll(JNIEnv *env, jobject anObject) {
	Trc_SC_MonitorNotifyAll_Entry(env, anObject);

	(*env)->CallVoidMethod(env, anObject, notifyAllMID);

	Trc_SC_MonitorNotifyAll_Exit(env);

}