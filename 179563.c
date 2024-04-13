static int af9005_pid_filter_control(struct dvb_usb_adapter *adap, int onoff)
{
	int ret;
	deb_info("pid filter control  onoff %d\n", onoff);
	if (onoff) {
		ret =
		    af9005_write_ofdm_register(adap->dev, XD_MP2IF_DMX_CTRL, 1);
		if (ret)
			return ret;
		ret =
		    af9005_write_register_bits(adap->dev,
					       XD_MP2IF_DMX_CTRL, 1, 1, 1);
		if (ret)
			return ret;
		ret =
		    af9005_write_ofdm_register(adap->dev, XD_MP2IF_DMX_CTRL, 1);
	} else
		ret =
		    af9005_write_ofdm_register(adap->dev, XD_MP2IF_DMX_CTRL, 0);
	if (ret)
		return ret;
	deb_info("pid filter control ok\n");
	return 0;
}