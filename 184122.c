DEFFUNC_llExecFunc(generateConfigDAGAction)
{
	action_t *pAction;
	uchar *pszModName;
	uchar *pszVertexName;
	struct dag_info *pDagInfo;
	DEFiRet;

	pDagInfo = (struct dag_info*) pParam;
	pAction = (action_t*) pData;

	pszModName = module.GetStateName(pAction->pMod);

	/* vertex */
	if(pAction->pszName == NULL) {
		if(!strcmp((char*)pszModName, "builtin-discard"))
			pszVertexName = (uchar*)"discard";
		else
			pszVertexName = pszModName;
	} else {
		pszVertexName = pAction->pszName;
	}

	fprintf(pDagInfo->fp, "\tact%d_%d\t\t[label=\"%s\"%s%s]\n",
		pDagInfo->iActUnit, pDagInfo->iAct, pszVertexName,
		pDagInfo->bDiscarded ? " style=dotted color=red" : "",
		(pAction->pQueue->qType == QUEUETYPE_DIRECT) ? "" : " shape=hexagon"
		);

	/* edge */
	if(pDagInfo->iAct == 0) {
	} else {
		fprintf(pDagInfo->fp, "\tact%d_%d -> act%d_%d[%s%s]\n",
			pDagInfo->iActUnit, pDagInfo->iAct - 1,
			pDagInfo->iActUnit, pDagInfo->iAct,
			pDagInfo->bDiscarded ? " style=dotted color=red" : "",
			pAction->bExecWhenPrevSusp ? " label=\"only if\\nsuspended\"" : "" );
	}

	/* check for discard */
	if(!strcmp((char*) pszModName, "builtin-discard")) {
		fprintf(pDagInfo->fp, "\tact%d_%d\t\t[shape=box]\n",
			pDagInfo->iActUnit, pDagInfo->iAct);
		pDagInfo->bDiscarded = 1;
	}


	++pDagInfo->iAct;

	RETiRet;
}