startInputModules(void)
{
	DEFiRet;
	modInfo_t *pMod;

	/* loop through all modules and activate them (brr...) */
	pMod = module.GetNxtType(NULL, eMOD_IN);
	while(pMod != NULL) {
		iRet = pMod->mod.im.willRun();
		pMod->mod.im.bCanRun = (iRet == RS_RET_OK);
		if(!pMod->mod.im.bCanRun) {
			DBGPRINTF("module %lx will not run, iRet %d\n", (unsigned long) pMod, iRet);
		}
	pMod = module.GetNxtType(pMod, eMOD_IN);
	}

	ENDfunc
	return RS_RET_OK; /* intentional: we do not care about module errors */
}