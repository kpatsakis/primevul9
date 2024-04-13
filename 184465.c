setZOSThrWeight(void)
{
	BOOLEAN success = FALSE;
	const UDATA jvmZOSTW = checkZOSThrWeightEnvVar();

	if (ZOS_THR_WEIGHT_NOT_FOUND != jvmZOSTW) {
		omrthread_t thandle = NULL;
		IDATA trc = 0;

		/* Attach so that we can use omrthread_global() below */
#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
		trc = omrthread_attach_ex(&thandle, J9THREAD_ATTR_DEFAULT);
#else
		trc = f_threadAttachEx(&thandle, J9THREAD_ATTR_DEFAULT);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

		if (0 == trc) {
			UDATA * gtw = NULL;

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
			gtw = omrthread_global("thread_weight");
#else
			gtw = f_threadGlobal("thread_weight");
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */

			if (NULL != gtw) {
				if (ZOS_THR_WEIGHT_HEAVY == jvmZOSTW) {
					*gtw = (UDATA)"heavy";
				} else {
					*gtw = (UDATA)"medium";
				}
				success = TRUE;
			}

#if CALL_BUNDLED_FUNCTIONS_DIRECTLY
			omrthread_detach(thandle);
#else
			f_threadDetach(thandle);
#endif /* CALL_BUNDLED_FUNCTIONS_DIRECTLY */
		}
	} else {
		success = TRUE;
	}

	return success;
}