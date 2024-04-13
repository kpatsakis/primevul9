static inline bool should_force_cow_break(struct vm_area_struct *vma, unsigned int flags)
{
	return is_cow_mapping(vma->vm_flags) && (flags & (FOLL_GET | FOLL_PIN));
}