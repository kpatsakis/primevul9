static void rtl8xxxu_calc_tx_desc_csum(struct rtl8xxxu_txdesc32 *tx_desc)
{
	__le16 *ptr = (__le16 *)tx_desc;
	u16 csum = 0;
	int i;

	/*
	 * Clear csum field before calculation, as the csum field is
	 * in the middle of the struct.
	 */
	tx_desc->csum = cpu_to_le16(0);

	for (i = 0; i < (sizeof(struct rtl8xxxu_txdesc32) / sizeof(u16)); i++)
		csum = csum ^ le16_to_cpu(ptr[i]);

	tx_desc->csum |= cpu_to_le16(csum);
}