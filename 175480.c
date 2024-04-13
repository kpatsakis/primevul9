static inline void __percpu *htab_elem_get_ptr(struct htab_elem *l, u32 key_size)
{
	return *(void __percpu **)(l->key + key_size);
}