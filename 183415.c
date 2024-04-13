				if(pData->dynBulkId) {
					CHKiRet(OMSRsetEntry(*ppOMSR, 1, ustrdup(pData->bulkId),
						OMSR_NO_RQD_TPL_OPTS));
				}