	if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
		errmsg.LogError(0, RS_RET_OBJ_CREATION_FAILED, "CURL fail. -elasticsearch indexing disabled");
		ABORT_FINALIZE(RS_RET_OBJ_CREATION_FAILED);
	}