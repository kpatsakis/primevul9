regional_init(struct regional* r)
{
	size_t a = ALIGN_UP(sizeof(struct regional), ALIGNMENT);
	r->data = (char*)r + a;
	r->available = r->first_size - a;
	r->next = NULL;
	r->large_list = NULL;
	r->total_large = 0;
}