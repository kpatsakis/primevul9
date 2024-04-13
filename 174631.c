static void ttusb_dec_set_model(struct ttusb_dec *dec,
				enum ttusb_dec_model model)
{
	dec->model = model;

	switch (model) {
	case TTUSB_DEC2000T:
		dec->model_name = "DEC2000-t";
		dec->firmware_name = "dvb-ttusb-dec-2000t.fw";
		break;

	case TTUSB_DEC2540T:
		dec->model_name = "DEC2540-t";
		dec->firmware_name = "dvb-ttusb-dec-2540t.fw";
		break;

	case TTUSB_DEC3000S:
		dec->model_name = "DEC3000-s";
		dec->firmware_name = "dvb-ttusb-dec-3000s.fw";
		break;
	}
}