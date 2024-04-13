static int netlink_ioctl(struct socket *sock, unsigned int cmd,
			 unsigned long arg)
{
	/* try to hand this ioctl down to the NIC drivers.
	 */
	return -ENOIOCTLCMD;
}