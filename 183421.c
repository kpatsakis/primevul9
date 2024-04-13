	if(pData->dynParent && pData->parent == NULL) {
		errmsg.LogError(0, RS_RET_CONFIG_ERROR,
			"omelasticsearch: requested dynamic parent, but no "
			"name for parent template given - action definition invalid");
		ABORT_FINALIZE(RS_RET_CONFIG_ERROR);
	}