sign_blob(cms_context *cms, SECItem *sigitem, SECItem *sign_content)
{
	sign_content = SECITEM_ArenaDupItem(cms->arena, sign_content);
	if (!sign_content)
		return -1;

	if (content_is_empty(sign_content->data, sign_content->len)) {
		cms->log(cms, LOG_ERR, "not signing empty digest");
		return -1;
	}

	SECOidData *oid = SECOID_FindOIDByTag(digest_get_signature_oid(cms));
	if (!oid)
		goto err;

	PK11_SetPasswordFunc(cms->func ? cms->func : SECU_GetModulePassword);
	SECKEYPrivateKey *privkey = PK11_FindKeyByAnyCert(cms->cert, cms);
	if (!privkey) {
		cms->log(cms, LOG_ERR, "could not get private key: %s",
			PORT_ErrorToString(PORT_GetError()));
		goto err;
	}

	SECItem *signature, tmp;
	memset (&tmp, '\0', sizeof (tmp));

	SECStatus status;
	status = SEC_SignData(&tmp, sign_content->data, sign_content->len,
			privkey, oid->offset);
	SECKEY_DestroyPrivateKey(privkey);
	privkey = NULL;

	if (status != SECSuccess) {
		cms->log(cms, LOG_ERR, "error signing data: %s",
			PORT_ErrorToString(PORT_GetError()));
		PORT_Free(tmp.data);
		return -1;
	}

	/* SEC_SignData awesomely allocates a SECItem and its contents for
	 * the signature, meaning they're not in our nss arena.  Fix it. */
	signature = SECITEM_AllocItem(cms->arena, NULL, tmp.len);
	if (!signature) {
		cms->log(cms, LOG_ERR, "error signing data: %s",
			PORT_ErrorToString(PORT_GetError()));
		return -1;
	}
	memcpy(signature->data, tmp.data, tmp.len);
	PORT_Free(tmp.data);

	if (status != SECSuccess) {
		cms->log(cms, LOG_ERR, "error signing data: %s",
			PORT_ErrorToString(PORT_GetError()));
		return -1;
	}
	memcpy(sigitem, signature, sizeof(*sigitem));

	//SECITEM_FreeItem(sign_content, PR_TRUE);
	return 0;
err:
	//SECITEM_FreeItem(sign_content, PR_TRUE);
	return -1;
}