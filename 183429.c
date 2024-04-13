	if(pData->dynSrchIdx) {
		CHKiRet(OMSRsetEntry(*ppOMSR, 1, ustrdup(pData->searchIndex),
			OMSR_NO_RQD_TPL_OPTS));
		if(pData->dynSrchType) {
			CHKiRet(OMSRsetEntry(*ppOMSR, 2, ustrdup(pData->searchType),
				OMSR_NO_RQD_TPL_OPTS));
			if(pData->dynParent) {
				CHKiRet(OMSRsetEntry(*ppOMSR, 3, ustrdup(pData->parent),
					OMSR_NO_RQD_TPL_OPTS));
				if(pData->dynBulkId) {
					CHKiRet(OMSRsetEntry(*ppOMSR, 4, ustrdup(pData->bulkId),
						OMSR_NO_RQD_TPL_OPTS));
				}
			} else {
				if(pData->dynBulkId) {
					CHKiRet(OMSRsetEntry(*ppOMSR, 3, ustrdup(pData->bulkId),
						OMSR_NO_RQD_TPL_OPTS));
				}
			}
		} else {
			if(pData->dynParent) {
				CHKiRet(OMSRsetEntry(*ppOMSR, 2, ustrdup(pData->parent),
					OMSR_NO_RQD_TPL_OPTS));
				if(pData->dynBulkId) {
					CHKiRet(OMSRsetEntry(*ppOMSR, 3, ustrdup(pData->bulkId),
						OMSR_NO_RQD_TPL_OPTS));
				}
			} else {
				if(pData->dynBulkId) {
					CHKiRet(OMSRsetEntry(*ppOMSR, 2, ustrdup(pData->bulkId),
						OMSR_NO_RQD_TPL_OPTS));
				}
			}
		}
	} else {