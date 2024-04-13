static inline struct fd fdget_pos(int fd)
{
	return __to_fd(__fdget_pos(fd));
}