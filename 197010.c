static int ems_usb_set_bittiming(struct net_device *netdev)
{
	struct ems_usb *dev = netdev_priv(netdev);
	struct can_bittiming *bt = &dev->can.bittiming;
	u8 btr0, btr1;

	btr0 = ((bt->brp - 1) & 0x3f) | (((bt->sjw - 1) & 0x3) << 6);
	btr1 = ((bt->prop_seg + bt->phase_seg1 - 1) & 0xf) |
		(((bt->phase_seg2 - 1) & 0x7) << 4);
	if (dev->can.ctrlmode & CAN_CTRLMODE_3_SAMPLES)
		btr1 |= 0x80;

	netdev_info(netdev, "setting BTR0=0x%02x BTR1=0x%02x\n", btr0, btr1);

	dev->active_params.msg.can_params.cc_params.sja1000.btr0 = btr0;
	dev->active_params.msg.can_params.cc_params.sja1000.btr1 = btr1;

	return ems_usb_command_msg(dev, &dev->active_params);
}