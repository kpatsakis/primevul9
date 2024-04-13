static int __init init_sys32_ioctl(void)
{
	int i;

	for (i = 0; i < ioctl_table_size; i++) {
		if (ioctl_start[i].next != 0) { 
			printk("ioctl translation %d bad\n",i); 
			return -1;
		}

		ioctl32_insert_translation(&ioctl_start[i]);
	}
	return 0;
}