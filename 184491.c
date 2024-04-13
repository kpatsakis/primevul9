addHarmonyPortLibrary(J9PortLibrary * portLib, J9JavaVMArgInfoList *vmArgumentsList, UDATA verboseFlags)
{
	JavaVMInitArgs dummyArgs = {0, 0, NULL, FALSE}; /* In this case, addHarmonyPortLibToVMArgs looks only at nOptions */
	J9JavaVMArgInfo *optArg = newJavaVMArgInfo(vmArgumentsList, NULL, CONSUMABLE_ARG);
	if (NULL == optArg) {
		return -1;
	}

	addHarmonyPortLibToVMArgs(portLib, &(optArg->vmOpt), &dummyArgs, &harmonyPortLibrary);
	return 0;
}