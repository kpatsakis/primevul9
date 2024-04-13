rtl8xxxu_set_spec_sifs(struct rtl8xxxu_priv *priv, u16 cck, u16 ofdm)
{
	u16 val16;

	val16 = ((cck << SPEC_SIFS_CCK_SHIFT) & SPEC_SIFS_CCK_MASK) |
		((ofdm << SPEC_SIFS_OFDM_SHIFT) & SPEC_SIFS_OFDM_MASK);

	rtl8xxxu_write16(priv, REG_SPEC_SIFS, val16);
}