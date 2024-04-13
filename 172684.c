static int btrfs_search_path_in_tree_user(struct inode *inode,
				struct btrfs_ioctl_ino_lookup_user_args *args)
{
	struct btrfs_fs_info *fs_info = BTRFS_I(inode)->root->fs_info;
	struct super_block *sb = inode->i_sb;
	struct btrfs_key upper_limit = BTRFS_I(inode)->location;
	u64 treeid = BTRFS_I(inode)->root->root_key.objectid;
	u64 dirid = args->dirid;
	unsigned long item_off;
	unsigned long item_len;
	struct btrfs_inode_ref *iref;
	struct btrfs_root_ref *rref;
	struct btrfs_root *root;
	struct btrfs_path *path;
	struct btrfs_key key, key2;
	struct extent_buffer *leaf;
	struct inode *temp_inode;
	char *ptr;
	int slot;
	int len;
	int total_len = 0;
	int ret;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	/*
	 * If the bottom subvolume does not exist directly under upper_limit,
	 * construct the path in from the bottom up.
	 */
	if (dirid != upper_limit.objectid) {
		ptr = &args->path[BTRFS_INO_LOOKUP_USER_PATH_MAX - 1];

		key.objectid = treeid;
		key.type = BTRFS_ROOT_ITEM_KEY;
		key.offset = (u64)-1;
		root = btrfs_read_fs_root_no_name(fs_info, &key);
		if (IS_ERR(root)) {
			ret = PTR_ERR(root);
			goto out;
		}

		key.objectid = dirid;
		key.type = BTRFS_INODE_REF_KEY;
		key.offset = (u64)-1;
		while (1) {
			ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
			if (ret < 0) {
				goto out;
			} else if (ret > 0) {
				ret = btrfs_previous_item(root, path, dirid,
							  BTRFS_INODE_REF_KEY);
				if (ret < 0) {
					goto out;
				} else if (ret > 0) {
					ret = -ENOENT;
					goto out;
				}
			}

			leaf = path->nodes[0];
			slot = path->slots[0];
			btrfs_item_key_to_cpu(leaf, &key, slot);

			iref = btrfs_item_ptr(leaf, slot, struct btrfs_inode_ref);
			len = btrfs_inode_ref_name_len(leaf, iref);
			ptr -= len + 1;
			total_len += len + 1;
			if (ptr < args->path) {
				ret = -ENAMETOOLONG;
				goto out;
			}

			*(ptr + len) = '/';
			read_extent_buffer(leaf, ptr,
					(unsigned long)(iref + 1), len);

			/* Check the read+exec permission of this directory */
			ret = btrfs_previous_item(root, path, dirid,
						  BTRFS_INODE_ITEM_KEY);
			if (ret < 0) {
				goto out;
			} else if (ret > 0) {
				ret = -ENOENT;
				goto out;
			}

			leaf = path->nodes[0];
			slot = path->slots[0];
			btrfs_item_key_to_cpu(leaf, &key2, slot);
			if (key2.objectid != dirid) {
				ret = -ENOENT;
				goto out;
			}

			temp_inode = btrfs_iget(sb, &key2, root, NULL);
			if (IS_ERR(temp_inode)) {
				ret = PTR_ERR(temp_inode);
				goto out;
			}
			ret = inode_permission(temp_inode, MAY_READ | MAY_EXEC);
			iput(temp_inode);
			if (ret) {
				ret = -EACCES;
				goto out;
			}

			if (key.offset == upper_limit.objectid)
				break;
			if (key.objectid == BTRFS_FIRST_FREE_OBJECTID) {
				ret = -EACCES;
				goto out;
			}

			btrfs_release_path(path);
			key.objectid = key.offset;
			key.offset = (u64)-1;
			dirid = key.objectid;
		}

		memmove(args->path, ptr, total_len);
		args->path[total_len] = '\0';
		btrfs_release_path(path);
	}

	/* Get the bottom subvolume's name from ROOT_REF */
	root = fs_info->tree_root;
	key.objectid = treeid;
	key.type = BTRFS_ROOT_REF_KEY;
	key.offset = args->treeid;
	ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
	if (ret < 0) {
		goto out;
	} else if (ret > 0) {
		ret = -ENOENT;
		goto out;
	}

	leaf = path->nodes[0];
	slot = path->slots[0];
	btrfs_item_key_to_cpu(leaf, &key, slot);

	item_off = btrfs_item_ptr_offset(leaf, slot);
	item_len = btrfs_item_size_nr(leaf, slot);
	/* Check if dirid in ROOT_REF corresponds to passed dirid */
	rref = btrfs_item_ptr(leaf, slot, struct btrfs_root_ref);
	if (args->dirid != btrfs_root_ref_dirid(leaf, rref)) {
		ret = -EINVAL;
		goto out;
	}

	/* Copy subvolume's name */
	item_off += sizeof(struct btrfs_root_ref);
	item_len -= sizeof(struct btrfs_root_ref);
	read_extent_buffer(leaf, args->name, item_off, item_len);
	args->name[item_len] = 0;

out:
	btrfs_free_path(path);
	return ret;
}