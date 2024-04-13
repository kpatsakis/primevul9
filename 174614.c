static void ttusb_dec_init_v_pes(struct ttusb_dec *dec)
{
	dprintk("%s\n", __func__);

	dec->v_pes[0] = 0x00;
	dec->v_pes[1] = 0x00;
	dec->v_pes[2] = 0x01;
	dec->v_pes[3] = 0xe0;
}