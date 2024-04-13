static int acrn_dev_release(struct inode *inode, struct file *filp)
{
	struct acrn_vm *vm = filp->private_data;

	acrn_vm_destroy(vm);
	kfree(vm);
	return 0;
}