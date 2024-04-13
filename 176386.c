static void raremono_device_release(struct v4l2_device *v4l2_dev)
{
	struct raremono_device *radio = to_raremono_dev(v4l2_dev);

	kfree(radio->buffer);
	kfree(radio);
}