static int usb_raremono_probe(struct usb_interface *intf,
				const struct usb_device_id *id)
{
	struct raremono_device *radio;
	int retval = 0;

	radio = kzalloc(sizeof(*radio), GFP_KERNEL);
	if (!radio)
		return -ENOMEM;
	radio->buffer = kmalloc(BUFFER_LENGTH, GFP_KERNEL);
	if (!radio->buffer) {
		kfree(radio);
		return -ENOMEM;
	}

	radio->usbdev = interface_to_usbdev(intf);
	radio->intf = intf;

	/*
	 * This device uses the same USB IDs as the si470x SiLabs reference
	 * design. So do an additional check: attempt to read the device ID
	 * from the si470x: the lower 12 bits are 0x0242 for the si470x. The
	 * Raremono always returns 0x0800 (the meaning of that is unknown, but
	 * at least it works).
	 *
	 * We use this check to determine which device we are dealing with.
	 */
	msleep(20);
	retval = usb_control_msg(radio->usbdev,
		usb_rcvctrlpipe(radio->usbdev, 0),
		HID_REQ_GET_REPORT,
		USB_TYPE_CLASS | USB_RECIP_INTERFACE | USB_DIR_IN,
		1, 2,
		radio->buffer, 3, 500);
	if (retval != 3 ||
	    (get_unaligned_be16(&radio->buffer[1]) & 0xfff) == 0x0242) {
		dev_info(&intf->dev, "this is not Thanko's Raremono.\n");
		retval = -ENODEV;
		goto free_mem;
	}

	dev_info(&intf->dev, "Thanko's Raremono connected: (%04X:%04X)\n",
			id->idVendor, id->idProduct);

	retval = v4l2_device_register(&intf->dev, &radio->v4l2_dev);
	if (retval < 0) {
		dev_err(&intf->dev, "couldn't register v4l2_device\n");
		goto free_mem;
	}

	mutex_init(&radio->lock);

	strscpy(radio->vdev.name, radio->v4l2_dev.name,
		sizeof(radio->vdev.name));
	radio->vdev.v4l2_dev = &radio->v4l2_dev;
	radio->vdev.fops = &usb_raremono_fops;
	radio->vdev.ioctl_ops = &usb_raremono_ioctl_ops;
	radio->vdev.lock = &radio->lock;
	radio->vdev.release = video_device_release_empty;
	radio->vdev.device_caps = V4L2_CAP_TUNER | V4L2_CAP_RADIO;
	radio->v4l2_dev.release = raremono_device_release;

	usb_set_intfdata(intf, &radio->v4l2_dev);

	video_set_drvdata(&radio->vdev, radio);

	raremono_cmd_main(radio, BAND_FM, 95160);

	retval = video_register_device(&radio->vdev, VFL_TYPE_RADIO, -1);
	if (retval == 0) {
		dev_info(&intf->dev, "V4L2 device registered as %s\n",
				video_device_node_name(&radio->vdev));
		return 0;
	}
	dev_err(&intf->dev, "could not register video device\n");
	v4l2_device_unregister(&radio->v4l2_dev);

free_mem:
	kfree(radio->buffer);
	kfree(radio);
	return retval;
}