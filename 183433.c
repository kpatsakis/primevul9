	if(pData->dynBulkId && pData->bulkId == NULL) {
		errmsg.LogError(0, RS_RET_CONFIG_ERROR,
			"omelasticsearch: requested dynamic bulkid, but no "
			"name for bulkid template given - action definition invalid");
		ABORT_FINALIZE(RS_RET_CONFIG_ERROR);
	}