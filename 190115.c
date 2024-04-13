static inline bool is_root_usable(struct kvm_mmu_root_info *root, gpa_t pgd,
				  union kvm_mmu_page_role role)
{
	return (role.direct || pgd == root->pgd) &&
	       VALID_PAGE(root->hpa) &&
	       role.word == to_shadow_page(root->hpa)->role.word;
}