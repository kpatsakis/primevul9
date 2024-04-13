void i40e_fill_rss_lut(struct i40e_pf *pf, u8 *lut,
		       u16 rss_table_size, u16 rss_size)
{
	u16 i;

	for (i = 0; i < rss_table_size; i++)
		lut[i] = i % rss_size;
}