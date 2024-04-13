static void ttusb_dec_exit_rc(struct ttusb_dec *dec)
{
	dprintk("%s\n", __func__);

	if (dec->rc_input_dev) {
		input_unregister_device(dec->rc_input_dev);
		dec->rc_input_dev = NULL;
	}
}