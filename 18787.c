out_flush_check(void)
{
    if (enc_dbcs != 0 && out_pos >= OUT_SIZE - MB_MAXBYTES)
	out_flush();
}