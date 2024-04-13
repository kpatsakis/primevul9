static int set_wep_key(struct adapter *padapter, u8 *key, u8 keylen, int keyid)
{
	u8 alg;

	switch (keylen) {
	case 5:
		alg = _WEP40_;
		break;
	case 13:
		alg = _WEP104_;
		break;
	default:
		alg = _NO_PRIVACY_;
	}

	return set_group_key(padapter, key, alg, keyid);
}