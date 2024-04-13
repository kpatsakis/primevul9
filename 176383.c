static void usb_raremono_disconnect(struct usb_interface *intf)
{
	struct raremono_device *radio = to_raremono_dev(usb_get_intfdata(intf));

	dev_info(&intf->dev, "Thanko's Raremono disconnected\n");

	mutex_lock(&radio->lock);
	usb_set_intfdata(intf, NULL);
	video_unregister_device(&radio->vdev);
	v4l2_device_disconnect(&radio->v4l2_dev);
	mutex_unlock(&radio->lock);
	v4l2_device_put(&radio->v4l2_dev);
}