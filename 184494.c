JVM_NanoTime(JNIEnv *env, jclass aClass)
{
	jlong ticks, freq;

	PORT_ACCESS_FROM_JAVAVM(BFUjavaVM);

	Trc_SC_NanoTime(env);

	ticks = j9time_hires_clock();
	freq = j9time_hires_frequency();

	/* freq is "ticks per s" */
	if ( freq == 1000000000L ) {
		return ticks;
	} else if ( freq < 1000000000L ) {
		return ticks * (1000000000L / freq);
	} else {
		return ticks / (freq / 1000000000L);
	}
}