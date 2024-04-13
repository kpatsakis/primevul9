find_named_certificate(cms_context *cms, char *name, CERTCertificate **cert)
{
	if (!name) {
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
			cms->log(cms, LOG_ERR, "could not find token \"%s\"",
				 cms->tokenname);
		}
		return -1;
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

	CERTCertListNode *node = NULL;
	for_each_cert(certlist, tmpnode) {
		if (!strcmp(tmpnode->cert->subjectName, name)) {
			node = tmpnode;
			break;
		}
	}
	/* If we're looking up the issuer of some cert, and the issuer isn't
	 * in the database, we'll get back what is essentially a template
	 * that's in NSS's cache waiting to be filled out.  We can't use that,
	 * it'll just cause CERT_DupCertificate() to segfault. */
	if (!node || !node->cert || !node->cert->derCert.data
	    || !node->cert->derCert.len
	    || !node->cert->derIssuer.data
	    || !node->cert->derIssuer.len) {
		PK11_DestroySlotListElement(slots, &psle);
		PK11_FreeSlotList(slots);
		CERT_DestroyCertList(certlist);

		return -1;
	}

	*cert = CERT_DupCertificate(node->cert);

	PK11_DestroySlotListElement(slots, &psle);
	PK11_FreeSlotList(slots);
	CERT_DestroyCertList(certlist);

	return 0;
}