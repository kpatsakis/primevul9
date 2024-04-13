ext4_expand_inode_array(struct ext4_xattr_inode_array **ea_inode_array,
			struct inode *inode)
{
	if (*ea_inode_array == NULL) {
		/*
		 * Start with 15 inodes, so it fits into a power-of-two size.
		 * If *ea_inode_array is NULL, this is essentially offsetof()
		 */
		(*ea_inode_array) =
			kmalloc(offsetof(struct ext4_xattr_inode_array,
					 inodes[EIA_MASK]),
				GFP_NOFS);
		if (*ea_inode_array == NULL)
			return -ENOMEM;
		(*ea_inode_array)->count = 0;
	} else if (((*ea_inode_array)->count & EIA_MASK) == EIA_MASK) {
		/* expand the array once all 15 + n * 16 slots are full */
		struct ext4_xattr_inode_array *new_array = NULL;
		int count = (*ea_inode_array)->count;

		/* if new_array is NULL, this is essentially offsetof() */
		new_array = kmalloc(
				offsetof(struct ext4_xattr_inode_array,
					 inodes[count + EIA_INCR]),
				GFP_NOFS);
		if (new_array == NULL)
			return -ENOMEM;
		memcpy(new_array, *ea_inode_array,
		       offsetof(struct ext4_xattr_inode_array, inodes[count]));
		kfree(*ea_inode_array);
		*ea_inode_array = new_array;
	}
	(*ea_inode_array)->inodes[(*ea_inode_array)->count++] = inode;
	return 0;
}