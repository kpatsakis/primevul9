static int rtw_wx_set_enc_ext(struct net_device *dev,
			      struct iw_request_info *info,
			      union iwreq_data *wrqu, char *extra)
{
	char *alg_name;
	u32 param_len;
	struct ieee_param *param = NULL;
	struct iw_point *pencoding = &wrqu->encoding;
	struct iw_encode_ext *pext = (struct iw_encode_ext *)extra;
	int ret = 0;

	param_len = sizeof(struct ieee_param) + pext->key_len;
	param = (struct ieee_param *)rtw_malloc(param_len);
	if (!param)
		return -1;

	memset(param, 0, param_len);

	param->cmd = IEEE_CMD_SET_ENCRYPTION;
	eth_broadcast_addr(param->sta_addr);

	switch (pext->alg) {
	case IW_ENCODE_ALG_NONE:
		/* todo: remove key */
		/* remove = 1; */
		alg_name = "none";
		break;
	case IW_ENCODE_ALG_WEP:
		alg_name = "WEP";
		break;
	case IW_ENCODE_ALG_TKIP:
		alg_name = "TKIP";
		break;
	case IW_ENCODE_ALG_CCMP:
		alg_name = "CCMP";
		break;
	default:
		ret = -1;
		goto exit;
	}

	strscpy((char *)param->u.crypt.alg, alg_name, IEEE_CRYPT_ALG_NAME_LEN);

	if (pext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY)
		param->u.crypt.set_tx = 1;

	/* cliW: WEP does not have group key
	 * just not checking GROUP key setting
	 */
	if ((pext->alg != IW_ENCODE_ALG_WEP) &&
	    (pext->ext_flags & IW_ENCODE_EXT_GROUP_KEY))
		param->u.crypt.set_tx = 0;

	param->u.crypt.idx = (pencoding->flags & 0x00FF) - 1;

	if (pext->ext_flags & IW_ENCODE_EXT_RX_SEQ_VALID)
		memcpy(param->u.crypt.seq, pext->rx_seq, 8);

	if (pext->key_len) {
		param->u.crypt.key_len = pext->key_len;
		memcpy(param->u.crypt.key, pext + 1, pext->key_len);
	}

	ret =  wpa_set_encryption(dev, param, param_len);

exit:
	kfree(param);
	return ret;
}