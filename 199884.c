static ssize_t snd_mem_proc_write(struct file *file, const char __user * buffer,
				  size_t count, loff_t * ppos)
{
	char buf[128];
	char *token, *p;

	if (count > sizeof(buf) - 1)
		return -EINVAL;
	if (copy_from_user(buf, buffer, count))
		return -EFAULT;
	buf[count] = '\0';

	p = buf;
	token = gettoken(&p);
	if (! token || *token == '#')
		return count;
	if (strcmp(token, "add") == 0) {
		char *endp;
		int vendor, device, size, buffers;
		long mask;
		int i, alloced;
		struct pci_dev *pci;

		if ((token = gettoken(&p)) == NULL ||
		    (vendor = simple_strtol(token, NULL, 0)) <= 0 ||
		    (token = gettoken(&p)) == NULL ||
		    (device = simple_strtol(token, NULL, 0)) <= 0 ||
		    (token = gettoken(&p)) == NULL ||
		    (mask = simple_strtol(token, NULL, 0)) < 0 ||
		    (token = gettoken(&p)) == NULL ||
		    (size = memparse(token, &endp)) < 64*1024 ||
		    size > 16*1024*1024 /* too big */ ||
		    (token = gettoken(&p)) == NULL ||
		    (buffers = simple_strtol(token, NULL, 0)) <= 0 ||
		    buffers > 4) {
			printk(KERN_ERR "snd-page-alloc: invalid proc write format\n");
			return count;
		}
		vendor &= 0xffff;
		device &= 0xffff;

		alloced = 0;
		pci = NULL;
		while ((pci = pci_get_device(vendor, device, pci)) != NULL) {
			if (mask > 0 && mask < 0xffffffff) {
				if (pci_set_dma_mask(pci, mask) < 0 ||
				    pci_set_consistent_dma_mask(pci, mask) < 0) {
					printk(KERN_ERR "snd-page-alloc: cannot set DMA mask %lx for pci %04x:%04x\n", mask, vendor, device);
					return count;
				}
			}
			for (i = 0; i < buffers; i++) {
				struct snd_dma_buffer dmab;
				memset(&dmab, 0, sizeof(dmab));
				if (snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV, snd_dma_pci_data(pci),
							size, &dmab) < 0) {
					printk(KERN_ERR "snd-page-alloc: cannot allocate buffer pages (size = %d)\n", size);
					pci_dev_put(pci);
					return count;
				}
				snd_dma_reserve_buf(&dmab, snd_dma_pci_buf_id(pci));
			}
			alloced++;
		}
		if (! alloced) {
			for (i = 0; i < buffers; i++) {
				struct snd_dma_buffer dmab;
				memset(&dmab, 0, sizeof(dmab));
				/* FIXME: We can allocate only in ZONE_DMA
				 * without a device pointer!
				 */
				if (snd_dma_alloc_pages(SNDRV_DMA_TYPE_DEV, NULL,
							size, &dmab) < 0) {
					printk(KERN_ERR "snd-page-alloc: cannot allocate buffer pages (size = %d)\n", size);
					break;
				}
				snd_dma_reserve_buf(&dmab, (unsigned int)((vendor << 16) | device));
			}
		}
	} else if (strcmp(token, "erase") == 0)
		/* FIXME: need for releasing each buffer chunk? */
		free_all_reserved_pages();
	else
		printk(KERN_ERR "snd-page-alloc: invalid proc cmd\n");
	return count;
}