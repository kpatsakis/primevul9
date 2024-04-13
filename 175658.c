static void gvt_unpin_guest_page(struct intel_vgpu *vgpu, unsigned long gfn,
		unsigned long size)
{
	int total_pages;
	int npage;
	int ret;

	total_pages = roundup(size, PAGE_SIZE) / PAGE_SIZE;

	for (npage = 0; npage < total_pages; npage++) {
		unsigned long cur_gfn = gfn + npage;

		ret = vfio_unpin_pages(mdev_dev(vgpu->vdev.mdev), &cur_gfn, 1);
		WARN_ON(ret != 1);
	}
}