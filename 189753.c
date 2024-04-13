static int show_smap(struct seq_file *m, void *v)
{
	struct vm_area_struct *vma = v;
	struct mem_size_stats mss;
	int ret;

	memset(&mss, 0, sizeof mss);
	mss.vma = vma;
	if (vma->vm_mm && !is_vm_hugetlb_page(vma))
		walk_page_range(vma->vm_mm, vma->vm_start, vma->vm_end,
				&smaps_walk, &mss);

	ret = show_map(m, v);
	if (ret)
		return ret;

	seq_printf(m,
		   "Size:           %8lu kB\n"
		   "Rss:            %8lu kB\n"
		   "Pss:            %8lu kB\n"
		   "Shared_Clean:   %8lu kB\n"
		   "Shared_Dirty:   %8lu kB\n"
		   "Private_Clean:  %8lu kB\n"
		   "Private_Dirty:  %8lu kB\n"
		   "Referenced:     %8lu kB\n",
		   (vma->vm_end - vma->vm_start) >> 10,
		   mss.resident >> 10,
		   (unsigned long)(mss.pss >> (10 + PSS_SHIFT)),
		   mss.shared_clean  >> 10,
		   mss.shared_dirty  >> 10,
		   mss.private_clean >> 10,
		   mss.private_dirty >> 10,
		   mss.referenced >> 10);

	return ret;
}