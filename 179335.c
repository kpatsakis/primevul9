static int rtw_wx_set_rate(struct net_device *dev,
			   struct iw_request_info *a,
			   union iwreq_data *wrqu, char *extra)
{
	int i;
	u8 datarates[NumRates];
	u32	target_rate = wrqu->bitrate.value;
	u32	fixed = wrqu->bitrate.fixed;
	u32	ratevalue = 0;
	u8 mpdatarate[NumRates] = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff};

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("%s\n", __func__));
	RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_, ("target_rate = %d, fixed = %d\n", target_rate, fixed));

	if (target_rate == -1) {
		ratevalue = 11;
		goto set_rate;
	}
	target_rate /= 100000;

	switch (target_rate) {
	case 10:
		ratevalue = 0;
		break;
	case 20:
		ratevalue = 1;
		break;
	case 55:
		ratevalue = 2;
		break;
	case 60:
		ratevalue = 3;
		break;
	case 90:
		ratevalue = 4;
		break;
	case 110:
		ratevalue = 5;
		break;
	case 120:
		ratevalue = 6;
		break;
	case 180:
		ratevalue = 7;
		break;
	case 240:
		ratevalue = 8;
		break;
	case 360:
		ratevalue = 9;
		break;
	case 480:
		ratevalue = 10;
		break;
	case 540:
		ratevalue = 11;
		break;
	default:
		ratevalue = 11;
		break;
	}

set_rate:

	for (i = 0; i < NumRates; i++) {
		if (ratevalue == mpdatarate[i]) {
			datarates[i] = mpdatarate[i];
			if (fixed == 0)
				break;
		} else {
			datarates[i] = 0xff;
		}

		RT_TRACE(_module_rtl871x_ioctl_os_c, _drv_info_, ("datarate_inx =%d\n", datarates[i]));
	}

	return 0;
}