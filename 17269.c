static inline unsigned int fold_hash(unsigned long hash)
{
	hash += hash >> (8*sizeof(int));
	return hash;
}