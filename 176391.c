static inline struct raremono_device *to_raremono_dev(struct v4l2_device *v4l2_dev)
{
	return container_of(v4l2_dev, struct raremono_device, v4l2_dev);
}