find_certificate(cms_context *cms, int needs_private_key)
{
	struct validity_cbdata cbd;
	if (!cms->certname || !*cms->certname) {
		cms->log(cms, LOG_ERR, "no certificate name specified");
		return -1;
	}

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
	if (PK11_NeedLogin(psle->slot) && !PK11_IsLoggedIn(psle->slot, cms)) {
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

	CERTCertList *certlist = NULL;
	certlist = PK11_ListCertsInSlot(psle->slot);
	if (!certlist) {
		save_port_err() {
			PK11_DestroySlotListElement(slots, &psle);
			PK11_FreeSlotList(slots);
		}
		cmsreterr(-1, cms, "could not get certificate list");
	}

	SECItem nickname = {
		.data = (void *)cms->certname,
		.len = strlen(cms->certname) + 1,
		.type = siUTF8String,
	};

	cms->psle = psle;

	cbd.cms = cms;
	cbd.psle = psle;
	cbd.slot = psle->slot;
	cbd.cert = NULL;

	if (needs_private_key) {
		status = PK11_TraverseCertsForNicknameInSlot(&nickname,
					psle->slot, is_valid_cert, &cbd);
	} else {
		status = PK11_TraverseCertsForNicknameInSlot(&nickname,
					psle->slot,
					is_valid_cert_without_private_key,
					&cbd);
	}
	if (status == SECSuccess && cbd.cert != NULL) {
		if (cms->cert)
			CERT_DestroyCertificate(cms->cert);
		cms->cert = CERT_DupCertificate(cbd.cert);
	}

	save_port_err() {
		CERT_DestroyCertList(certlist);
		PK11_DestroySlotListElement(slots, &psle);
		PK11_FreeSlotList(slots);
		cms->psle = NULL;
	}
	if (status != SECSuccess || cms->cert == NULL)
		cmsreterr(-1, cms, "could not find certificate in list");

	return 0;
}