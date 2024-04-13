static inline void f2fs_copy_page(struct page *src, struct page *dst)
{
	char *src_kaddr = kmap(src);
	char *dst_kaddr = kmap(dst);

	memcpy(dst_kaddr, src_kaddr, PAGE_SIZE);
	kunmap(dst);
	kunmap(src);
}