rtl8xxxu_set_retry(struct rtl8xxxu_priv *priv, u16 short_retry, u16 long_retry)
{
	u16 val16;

	val16 = ((short_retry << RETRY_LIMIT_SHORT_SHIFT) &
		 RETRY_LIMIT_SHORT_MASK) |
		((long_retry << RETRY_LIMIT_LONG_SHIFT) &
		 RETRY_LIMIT_LONG_MASK);

	rtl8xxxu_write16(priv, REG_RETRY_LIMIT, val16);
}