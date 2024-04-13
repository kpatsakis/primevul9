static inline unsigned int uffd_ctx_features(__u64 user_features)
{
	/*
	 * For the current set of features the bits just coincide
	 */
	return (unsigned int)user_features;
}