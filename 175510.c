static inline void htab_elem_set_ptr(struct htab_elem *l, u32 key_size,
				     void __percpu *pptr)
{
	*(void __percpu **)(l->key + key_size) = pptr;
}