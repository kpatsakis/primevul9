kfree_sgl(MptSge_t *sgl, dma_addr_t sgl_dma, struct buflist *buflist, MPT_ADAPTER *ioc)
{
	MptSge_t	*sg = sgl;
	struct buflist	*bl = buflist;
	u32		 nib;
	int		 dir;
	int		 n = 0;

	if (sg->FlagsLength & 0x04000000)
		dir = PCI_DMA_TODEVICE;
	else
		dir = PCI_DMA_FROMDEVICE;

	nib = (sg->FlagsLength & 0xF0000000) >> 28;
	while (! (nib & 0x4)) { /* eob */
		/* skip ignore/chain. */
		if (nib == 0 || nib == 3) {
			;
		} else if (sg->Address) {
			dma_addr_t dma_addr;
			void *kptr;
			int len;

			dma_addr = sg->Address;
			kptr = bl->kptr;
			len = bl->len;
			pci_unmap_single(ioc->pcidev, dma_addr, len, dir);
			pci_free_consistent(ioc->pcidev, len, kptr, dma_addr);
			n++;
		}
		sg++;
		bl++;
		nib = (le32_to_cpu(sg->FlagsLength) & 0xF0000000) >> 28;
	}

	/* we're at eob! */
	if (sg->Address) {
		dma_addr_t dma_addr;
		void *kptr;
		int len;

		dma_addr = sg->Address;
		kptr = bl->kptr;
		len = bl->len;
		pci_unmap_single(ioc->pcidev, dma_addr, len, dir);
		pci_free_consistent(ioc->pcidev, len, kptr, dma_addr);
		n++;
	}

	pci_free_consistent(ioc->pcidev, MAX_SGL_BYTES, sgl, sgl_dma);
	kfree(buflist);
	dctlprintk(ioc, printk(MYIOC_s_DEBUG_FMT "-SG: Free'd 1 SGL buf + %d kbufs!\n",
	    ioc->name, n));
}