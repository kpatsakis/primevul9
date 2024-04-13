static struct page *linear_to_page(struct page *page, unsigned int *len,
				   unsigned int *offset,
				   struct sock *sk)
{
	struct page_frag *pfrag = sk_page_frag(sk);

	if (!sk_page_frag_refill(sk, pfrag))
		return NULL;

	*len = min_t(unsigned int, *len, pfrag->size - pfrag->offset);

	memcpy(page_address(pfrag->page) + pfrag->offset,
	       page_address(page) + *offset, *len);
	*offset = pfrag->offset;
	pfrag->offset += *len;

	return pfrag->page;
}