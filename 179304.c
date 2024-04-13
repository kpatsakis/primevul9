static int set_group_key(struct adapter *padapter, u8 *key, u8 alg, int keyid)
{
	u8 keylen;
	struct cmd_obj *pcmd;
	struct setkey_parm *psetkeyparm;
	struct cmd_priv	*pcmdpriv = &padapter->cmdpriv;
	int res = _SUCCESS;

	DBG_88E("%s\n", __func__);

	pcmd = kzalloc(sizeof(struct	cmd_obj), GFP_KERNEL);
	if (!pcmd) {
		res = _FAIL;
		goto exit;
	}
	psetkeyparm = kzalloc(sizeof(struct setkey_parm), GFP_KERNEL);
	if (!psetkeyparm) {
		kfree(pcmd);
		res = _FAIL;
		goto exit;
	}

	psetkeyparm->keyid = (u8)keyid;

	psetkeyparm->algorithm = alg;

	psetkeyparm->set_tx = 1;

	switch (alg) {
	case _WEP40_:
		keylen = 5;
		break;
	case _WEP104_:
		keylen = 13;
		break;
	case _TKIP_:
	case _TKIP_WTMIC_:
	case _AES_:
	default:
		keylen = 16;
	}

	memcpy(&psetkeyparm->key[0], key, keylen);

	pcmd->cmdcode = _SetKey_CMD_;
	pcmd->parmbuf = (u8 *)psetkeyparm;
	pcmd->cmdsz =  (sizeof(struct setkey_parm));
	pcmd->rsp = NULL;
	pcmd->rspsz = 0;

	INIT_LIST_HEAD(&pcmd->list);

	res = rtw_enqueue_cmd(pcmdpriv, pcmd);

exit:

	return res;
}