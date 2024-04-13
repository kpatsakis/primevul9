checkZOSThrWeightEnvVar(void)
{
	UDATA retVal = ZOS_THR_WEIGHT_NOT_FOUND;
#pragma convlit(suspend)
#undef getenv
	const char * const val = getenv("JAVA_THREAD_MODEL");

	if (NULL != val) {
		/*
		 * If the customer did not request heavy weight, assume medium.
		 * Note that the goal here is not to properly parse the env 
		 * var. This is done in threadParseArguments() and it will flag
		 * if the customer attempts to pass a bad value in the env var.
		 */
		if (0 == strcmp(val, "HEAVY")) {
#pragma convlit(resume)
			retVal = ZOS_THR_WEIGHT_HEAVY;
		} else {
			retVal = ZOS_THR_WEIGHT_MEDIUM;
		}
	}

	return retVal;
}