			if(pData->dynParent) {
				CHKiRet(OMSRsetEntry(*ppOMSR, 1, ustrdup(pData->parent),
					OMSR_NO_RQD_TPL_OPTS));
                if(pData->dynBulkId) {
                    CHKiRet(OMSRsetEntry(*ppOMSR, 2, ustrdup(pData->bulkId),
                        OMSR_NO_RQD_TPL_OPTS));
                }
			} else {