JVM_FindSignal(const char *sigName)
{
	const J9SignalMapping *mapping = NULL;
	jint signalValue = J9_SIG_ERR;
	BOOLEAN nameHasSigPrefix = FALSE;
	const char *fullSigName = sigName;
#if !defined(WIN32)
	char nameWithSIGPrefix[J9_SIGNAME_BUFFER_LENGTH] = {0};
#endif /* !defined(WIN32) */

	Trc_SC_FindSignal_Entry(sigName);

	if (NULL != sigName) {
		size_t sigPrefixLength = sizeof(J9_SIG_PREFIX) - 1;

#if !defined(WIN32)
		if (0 != strncmp(sigName, J9_SIG_PREFIX, sigPrefixLength)) {
			/* nameWithSIGPrefix is a char buffer of length J9_SIGNAME_BUFFER_LENGTH.
			 * We are concatenating SIG + sigName, and storing the new string in
			 * nameWithSIGPrefix. We want to make sure that the concatenated string
			 * fits inside nameWithSIGPrefix. We also know that all known signal names
			 * have less than J9_SIGNAME_BUFFER_LENGTH chars. If the concatenated
			 * string doesn't fit inside nameWithSIGPrefix, then we can consider signal
			 * name to be unknown. In sigNameLength, +1 is for the NULL terminator.
			 */
			size_t sigNameLength = sigPrefixLength + strlen(sigName) + 1;
			if (sigNameLength <= J9_SIGNAME_BUFFER_LENGTH) {
				strcpy(nameWithSIGPrefix, J9_SIG_PREFIX);
				strcat(nameWithSIGPrefix, sigName);
				fullSigName = nameWithSIGPrefix;
			} else {
				goto exit;
			}
		}
#endif /* !defined(WIN32) */

		for (mapping = signalMap; NULL != mapping->signalName; mapping++) {
			if (0 == strcmp(fullSigName, mapping->signalName)) {
				signalValue = mapping->signalValue;
				break;
			}
		}
	}

#if !defined(WIN32)
exit:
#endif /* !defined(WIN32) */
	Trc_SC_FindSignal_Exit(signalValue);
	return signalValue;
}