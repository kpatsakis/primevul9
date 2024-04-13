static void tw5864_encoder_tables_upload(struct tw5864_dev *dev)
{
	int i;

	tw_writel(TW5864_VLC_RD, 0x1);
	for (i = 0; i < VLC_LOOKUP_TABLE_LEN; i++) {
		tw_writel((TW5864_VLC_STREAM_MEM_START + i * 4),
			  encoder_vlc_lookup_table[i]);
	}
	tw_writel(TW5864_VLC_RD, 0x0);

	for (i = 0; i < QUANTIZATION_TABLE_LEN; i++) {
		tw_writel((TW5864_QUAN_TAB + i * 4),
			  forward_quantization_table[i]);
	}

	for (i = 0; i < QUANTIZATION_TABLE_LEN; i++) {
		tw_writel((TW5864_QUAN_TAB + i * 4),
			  inverse_quantization_table[i]);
	}
}