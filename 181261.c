nfsd4_getdeviceinfo_release(union nfsd4_op_u *u)
{
	kfree(u->getdeviceinfo.gd_device);
}