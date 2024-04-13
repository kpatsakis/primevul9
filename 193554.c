static long vhost_net_ioctl(struct file *f, unsigned int ioctl,
			    unsigned long arg)
{
	struct vhost_net *n = f->private_data;
	void __user *argp = (void __user *)arg;
	u64 __user *featurep = argp;
	struct vhost_vring_file backend;
	u64 features;
	int r;

	switch (ioctl) {
	case VHOST_NET_SET_BACKEND:
		if (copy_from_user(&backend, argp, sizeof backend))
			return -EFAULT;
		return vhost_net_set_backend(n, backend.index, backend.fd);
	case VHOST_GET_FEATURES:
		features = VHOST_NET_FEATURES;
		if (copy_to_user(featurep, &features, sizeof features))
			return -EFAULT;
		return 0;
	case VHOST_SET_FEATURES:
		if (copy_from_user(&features, featurep, sizeof features))
			return -EFAULT;
		if (features & ~VHOST_NET_FEATURES)
			return -EOPNOTSUPP;
		return vhost_net_set_features(n, features);
	case VHOST_RESET_OWNER:
		return vhost_net_reset_owner(n);
	default:
		mutex_lock(&n->dev.mutex);
		r = vhost_dev_ioctl(&n->dev, ioctl, argp);
		if (r == -ENOIOCTLCMD)
			r = vhost_vring_ioctl(&n->dev, ioctl, argp);
		else
			vhost_net_flush(n);
		mutex_unlock(&n->dev.mutex);
		return r;
	}
}