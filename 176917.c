static int pcan_usb_pro_set_bittiming(struct peak_usb_device *dev,
				      struct can_bittiming *bt)
{
	u32 ccbt;

	ccbt = (dev->can.ctrlmode & CAN_CTRLMODE_3_SAMPLES) ? 0x00800000 : 0;
	ccbt |= (bt->sjw - 1) << 24;
	ccbt |= (bt->phase_seg2 - 1) << 20;
	ccbt |= (bt->prop_seg + bt->phase_seg1 - 1) << 16; /* = tseg1 */
	ccbt |= bt->brp - 1;

	netdev_info(dev->netdev, "setting ccbt=0x%08x\n", ccbt);

	return pcan_usb_pro_set_bitrate(dev, ccbt);
}