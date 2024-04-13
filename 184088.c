runInputModules(void)
{
	modInfo_t *pMod;

	BEGINfunc
	/* loop through all modules and activate them (brr...) */
	pMod = module.GetNxtType(NULL, eMOD_IN);
	while(pMod != NULL) {
		if(pMod->mod.im.bCanRun) {
			/* activate here */
			thrdCreate(pMod->mod.im.runInput, pMod->mod.im.afterRun);
		}
	pMod = module.GetNxtType(pMod, eMOD_IN);
	}

	ENDfunc
	return RS_RET_OK; /* intentional: we do not care about module errors */
}