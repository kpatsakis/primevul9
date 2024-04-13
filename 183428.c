checkResult(instanceData *pData, uchar *reqmsg)
{
	cJSON *root;
	cJSON *ok;
	DEFiRet;

	root = cJSON_Parse(pData->reply);
	if(root == NULL) {
		DBGPRINTF("omelasticsearch: could not parse JSON result \n");
		ABORT_FINALIZE(RS_RET_ERR);
	}

	if(pData->bulkmode) {
		iRet = checkResultBulkmode(pData, root);
	} else {
		ok = cJSON_GetObjectItem(root, "ok");
		if(ok == NULL || ok->type != cJSON_True) {
			iRet = RS_RET_DATAFAIL;
		}
	}

	/* Note: we ignore errors writing the error file, as we cannot handle
	 * these in any case.
	 */
	if(iRet == RS_RET_DATAFAIL) {
		writeDataError(pData, &root, reqmsg);
		iRet = RS_RET_OK; /* we have handled the problem! */
	}

finalize_it:
	if(root != NULL)
		cJSON_Delete(root);
	RETiRet;
}