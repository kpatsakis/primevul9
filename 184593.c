JVM_Sleep(JNIEnv* env, jclass thread, jlong timeout)
{
	Trc_SC_Sleep_Entry(env, thread, timeout);

	(*env)->CallStaticVoidMethod(env, jlThread, sleepMID, timeout);

	Trc_SC_Sleep_Exit(env);

	return 0;
}