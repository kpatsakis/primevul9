static int rpmsg_ctrldev_open(struct inode *inode, struct file *filp)
{
	struct rpmsg_ctrldev *ctrldev = cdev_to_ctrldev(inode->i_cdev);

	get_device(&ctrldev->dev);
	filp->private_data = ctrldev;

	return 0;
}