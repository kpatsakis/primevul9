printVmArgumentsList(J9VMInitArgs *argList)
{
	UDATA i;
	JavaVMInitArgs *actualArgs = argList->actualVMArgs;
	for (i = 0; i < argList->nOptions; ++i) {
		J9CmdLineOption* j9Option = &(argList->j9Options[i]);
		char *envVar = j9Option->fromEnvVar;

		if (NULL == envVar) {
			envVar = "N/A";
		}
		fprintf(stderr, "Option %" J9PRIz "d optionString=\"%s\" extraInfo=%p from environment variable =\"%s\"\n", i,
				actualArgs->options[i].optionString,
				actualArgs->options[i].extraInfo,
				envVar);
	}
}