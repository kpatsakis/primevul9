	if(pData->pwd != NULL && pData->uid == NULL) {
		errmsg.LogError(0, RS_RET_UID_MISSING,
			"omelasticsearch: password is provided, but no uid "
			"- action definition invalid");
		ABORT_FINALIZE(RS_RET_UID_MISSING);
	}