static inline bool f2fs_has_xattr_block(unsigned int ofs)
{
	return ofs == XATTR_NODE_OFFSET;
}