static inline bool vma_can_userfault(struct vm_area_struct *vma)
{
	return vma_is_anonymous(vma) || is_vm_hugetlb_page(vma) ||
		vma_is_shmem(vma);
}