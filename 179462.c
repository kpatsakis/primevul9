 */
static __must_check inline bool user_access_begin(const void __user *ptr, size_t len)
{
	if (unlikely(!access_ok(ptr,len)))
		return 0;
	__uaccess_begin();
	return 1;