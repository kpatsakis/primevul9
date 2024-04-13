static int gvt_pin_guest_page(struct intel_vgpu *vgpu, unsigned long gfn,
		unsigned long size, struct page **page)
{
	unsigned long base_pfn = 0;
	int total_pages;
	int npage;
	int ret;

	total_pages = roundup(size, PAGE_SIZE) / PAGE_SIZE;
	/*
	 * We pin the pages one-by-one to avoid allocating a big arrary
	 * on stack to hold pfns.
	 */
	for (npage = 0; npage < total_pages; npage++) {
		unsigned long cur_gfn = gfn + npage;
		unsigned long pfn;

		ret = vfio_pin_pages(mdev_dev(vgpu->vdev.mdev), &cur_gfn, 1,
				     IOMMU_READ | IOMMU_WRITE, &pfn);
		if (ret != 1) {
			gvt_vgpu_err("vfio_pin_pages failed for gfn 0x%lx, ret %d\n",
				     cur_gfn, ret);
			goto err;
		}

		if (!pfn_valid(pfn)) {
			gvt_vgpu_err("pfn 0x%lx is not mem backed\n", pfn);
			npage++;
			ret = -EFAULT;
			goto err;
		}

		if (npage == 0)
			base_pfn = pfn;
		else if (base_pfn + npage != pfn) {
			gvt_vgpu_err("The pages are not continuous\n");
			ret = -EINVAL;
			npage++;
			goto err;
		}
	}

	*page = pfn_to_page(base_pfn);
	return 0;
err:
	gvt_unpin_guest_page(vgpu, gfn, npage * PAGE_SIZE);
	return ret;
}