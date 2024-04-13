init_sg(void)
{
	int rc;

	if (scatter_elem_sz < PAGE_SIZE) {
		scatter_elem_sz = PAGE_SIZE;
		scatter_elem_sz_prev = scatter_elem_sz;
	}
	if (def_reserved_size >= 0)
		sg_big_buff = def_reserved_size;
	else
		def_reserved_size = sg_big_buff;

	rc = register_chrdev_region(MKDEV(SCSI_GENERIC_MAJOR, 0),
				    SG_MAX_DEVS, "sg");
	if (rc)
		return rc;
        sg_sysfs_class = class_create(THIS_MODULE, "scsi_generic");
        if ( IS_ERR(sg_sysfs_class) ) {
		rc = PTR_ERR(sg_sysfs_class);
		goto err_out;
        }
	sg_sysfs_valid = 1;
	rc = scsi_register_interface(&sg_interface);
	if (0 == rc) {
#ifdef CONFIG_SCSI_PROC_FS
		sg_proc_init();
#endif				/* CONFIG_SCSI_PROC_FS */
		return 0;
	}
	class_destroy(sg_sysfs_class);
err_out:
	unregister_chrdev_region(MKDEV(SCSI_GENERIC_MAJOR, 0), SG_MAX_DEVS);
	return rc;
}