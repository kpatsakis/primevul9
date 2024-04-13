static u8 set_pairwise_key(struct adapter *padapter, struct sta_info *psta)
{
	struct cmd_obj *ph2c;
	struct set_stakey_parm	*psetstakey_para;
	struct cmd_priv	*pcmdpriv = &padapter->cmdpriv;
	u8 res = _SUCCESS;

	ph2c = kzalloc(sizeof(struct cmd_obj), GFP_KERNEL);
	if (!ph2c) {
		res = _FAIL;
		goto exit;
	}

	psetstakey_para = kzalloc(sizeof(struct set_stakey_parm), GFP_KERNEL);
	if (!psetstakey_para) {
		kfree(ph2c);
		res = _FAIL;
		goto exit;
	}

	init_h2fwcmd_w_parm_no_rsp(ph2c, psetstakey_para, _SetStaKey_CMD_);

	psetstakey_para->algorithm = (u8)psta->dot118021XPrivacy;

	memcpy(psetstakey_para->addr, psta->hwaddr, ETH_ALEN);

	memcpy(psetstakey_para->key, &psta->dot118021x_UncstKey, 16);

	res = rtw_enqueue_cmd(pcmdpriv, ph2c);

exit:

	return res;
}