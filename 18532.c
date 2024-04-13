unlock_nss_token(cms_context *cms)
{
	PK11_SetPasswordFunc(cms->func ? cms->func : SECU_GetModulePassword);

	PK11SlotList *slots = NULL;
	slots = PK11_GetAllTokens(CKM_RSA_PKCS, PR_FALSE, PR_TRUE, cms);
	if (!slots)
		cmsreterr(-1, cms, "could not get pk11 token list");

	PK11SlotListElement *psle = NULL;
	psle = PK11_GetFirstSafe(slots);
	if (!psle) {
		save_port_err() {
			PK11_FreeSlotList(slots);
		}
		cmsreterr(-1, cms, "could not get pk11 safe");
	}

	while (psle) {
		if (!strcmp(cms->tokenname, PK11_GetTokenName(psle->slot)))
			break;

		psle = PK11_GetNextSafe(slots, psle, PR_FALSE);
	}

	if (!psle) {
		save_port_err() {
			PK11_FreeSlotList(slots);
		}
		nssreterr(-1, "Could not find token \"%s\"", cms->tokenname);
	}

	SECStatus status;
	if (PK11_NeedLogin(psle->slot) &&
			!PK11_IsLoggedIn(psle->slot, cms)) {
		status = PK11_Authenticate(psle->slot, PR_TRUE, cms);
		if (status != SECSuccess) {
			save_port_err() {
				PK11_DestroySlotListElement(slots, &psle);
				PK11_FreeSlotList(slots);
				cms->log(cms, LOG_ERR,
					 "authentication failed for token \"%s\"",
					 cms->tokenname);
			}
			return -1;
		}
	}

	PK11_DestroySlotListElement(slots, &psle);
	PK11_FreeSlotList(slots);
	return 0;
}