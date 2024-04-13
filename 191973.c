rtl8xxxu_set_ampdu_factor(struct rtl8xxxu_priv *priv, u8 ampdu_factor)
{
	u8 vals[4] = { 0x41, 0xa8, 0x72, 0xb9 };
	u8 max_agg = 0xf;
	int i;

	ampdu_factor = 1 << (ampdu_factor + 2);
	if (ampdu_factor > max_agg)
		ampdu_factor = max_agg;

	for (i = 0; i < 4; i++) {
		if ((vals[i] & 0xf0) > (ampdu_factor << 4))
			vals[i] = (vals[i] & 0x0f) | (ampdu_factor << 4);

		if ((vals[i] & 0x0f) > ampdu_factor)
			vals[i] = (vals[i] & 0xf0) | ampdu_factor;

		rtl8xxxu_write8(priv, REG_AGGLEN_LMT + i, vals[i]);
	}
}