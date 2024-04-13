static inline struct file *__fget(unsigned int fd, fmode_t mask,
				  unsigned int refs)
{
	return __fget_files(current->files, fd, mask, refs);
}