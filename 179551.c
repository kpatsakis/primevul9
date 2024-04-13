static int af9005_pid_filter(struct dvb_usb_adapter *adap, int index,
			     u16 pid, int onoff)
{
	u8 cmd = index & 0x1f;
	int ret;
	deb_info("set pid filter, index %d, pid %x, onoff %d\n", index,
		 pid, onoff);
	if (onoff) {
		/* cannot use it as pid_filter_ctrl since it has to be done
		   before setting the first pid */
		if (adap->feedcount == 1) {
			deb_info("first pid set, enable pid table\n");
			ret = af9005_pid_filter_control(adap, onoff);
			if (ret)
				return ret;
		}
		ret =
		    af9005_write_ofdm_register(adap->dev,
					       XD_MP2IF_PID_DATA_L,
					       (u8) (pid & 0xff));
		if (ret)
			return ret;
		ret =
		    af9005_write_ofdm_register(adap->dev,
					       XD_MP2IF_PID_DATA_H,
					       (u8) (pid >> 8));
		if (ret)
			return ret;
		cmd |= 0x20 | 0x40;
	} else {
		if (adap->feedcount == 0) {
			deb_info("last pid unset, disable pid table\n");
			ret = af9005_pid_filter_control(adap, onoff);
			if (ret)
				return ret;
		}
	}
	ret = af9005_write_ofdm_register(adap->dev, XD_MP2IF_PID_IDX, cmd);
	if (ret)
		return ret;
	deb_info("set pid ok\n");
	return 0;
}