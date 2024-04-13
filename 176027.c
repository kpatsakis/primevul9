static inline void msg_init(struct uffd_msg *msg)
{
	BUILD_BUG_ON(sizeof(struct uffd_msg) != 32);
	/*
	 * Must use memset to zero out the paddings or kernel data is
	 * leaked to userland.
	 */
	memset(msg, 0, sizeof(struct uffd_msg));
}