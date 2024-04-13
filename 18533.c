is_valid_cert(CERTCertificate *cert, void *data)
{
	struct validity_cbdata *cbd = (struct validity_cbdata *)data;
	PK11SlotInfo *slot = cbd->slot;
	SECKEYPrivateKey *privkey = NULL;

	if (cert == NULL)
		return SECFailure;

	privkey = PK11_FindPrivateKeyFromCert(slot, cert, cbd->cms);
	if (privkey != NULL) {
		cbd->cert = CERT_DupCertificate(cert);
		SECKEY_DestroyPrivateKey(privkey);
		return SECSuccess;
	}
	return SECFailure;
}