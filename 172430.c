static int btrfs_ioctl_get_subvol_info(struct file *file, void __user *argp)
{
	struct btrfs_ioctl_get_subvol_info_args *subvol_info;
	struct btrfs_fs_info *fs_info;
	struct btrfs_root *root;
	struct btrfs_path *path;
	struct btrfs_key key;
	struct btrfs_root_item *root_item;
	struct btrfs_root_ref *rref;
	struct extent_buffer *leaf;
	unsigned long item_off;
	unsigned long item_len;
	struct inode *inode;
	int slot;
	int ret = 0;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	subvol_info = kzalloc(sizeof(*subvol_info), GFP_KERNEL);
	if (!subvol_info) {
		btrfs_free_path(path);
		return -ENOMEM;
	}

	inode = file_inode(file);
	fs_info = BTRFS_I(inode)->root->fs_info;

	/* Get root_item of inode's subvolume */
	key.objectid = BTRFS_I(inode)->root->root_key.objectid;
	key.type = BTRFS_ROOT_ITEM_KEY;
	key.offset = (u64)-1;
	root = btrfs_read_fs_root_no_name(fs_info, &key);
	if (IS_ERR(root)) {
		ret = PTR_ERR(root);
		goto out;
	}
	root_item = &root->root_item;

	subvol_info->treeid = key.objectid;

	subvol_info->generation = btrfs_root_generation(root_item);
	subvol_info->flags = btrfs_root_flags(root_item);

	memcpy(subvol_info->uuid, root_item->uuid, BTRFS_UUID_SIZE);
	memcpy(subvol_info->parent_uuid, root_item->parent_uuid,
						    BTRFS_UUID_SIZE);
	memcpy(subvol_info->received_uuid, root_item->received_uuid,
						    BTRFS_UUID_SIZE);

	subvol_info->ctransid = btrfs_root_ctransid(root_item);
	subvol_info->ctime.sec = btrfs_stack_timespec_sec(&root_item->ctime);
	subvol_info->ctime.nsec = btrfs_stack_timespec_nsec(&root_item->ctime);

	subvol_info->otransid = btrfs_root_otransid(root_item);
	subvol_info->otime.sec = btrfs_stack_timespec_sec(&root_item->otime);
	subvol_info->otime.nsec = btrfs_stack_timespec_nsec(&root_item->otime);

	subvol_info->stransid = btrfs_root_stransid(root_item);
	subvol_info->stime.sec = btrfs_stack_timespec_sec(&root_item->stime);
	subvol_info->stime.nsec = btrfs_stack_timespec_nsec(&root_item->stime);

	subvol_info->rtransid = btrfs_root_rtransid(root_item);
	subvol_info->rtime.sec = btrfs_stack_timespec_sec(&root_item->rtime);
	subvol_info->rtime.nsec = btrfs_stack_timespec_nsec(&root_item->rtime);

	if (key.objectid != BTRFS_FS_TREE_OBJECTID) {
		/* Search root tree for ROOT_BACKREF of this subvolume */
		root = fs_info->tree_root;

		key.type = BTRFS_ROOT_BACKREF_KEY;
		key.offset = 0;
		ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
		if (ret < 0) {
			goto out;
		} else if (path->slots[0] >=
			   btrfs_header_nritems(path->nodes[0])) {
			ret = btrfs_next_leaf(root, path);
			if (ret < 0) {
				goto out;
			} else if (ret > 0) {
				ret = -EUCLEAN;
				goto out;
			}
		}

		leaf = path->nodes[0];
		slot = path->slots[0];
		btrfs_item_key_to_cpu(leaf, &key, slot);
		if (key.objectid == subvol_info->treeid &&
		    key.type == BTRFS_ROOT_BACKREF_KEY) {
			subvol_info->parent_id = key.offset;

			rref = btrfs_item_ptr(leaf, slot, struct btrfs_root_ref);
			subvol_info->dirid = btrfs_root_ref_dirid(leaf, rref);

			item_off = btrfs_item_ptr_offset(leaf, slot)
					+ sizeof(struct btrfs_root_ref);
			item_len = btrfs_item_size_nr(leaf, slot)
					- sizeof(struct btrfs_root_ref);
			read_extent_buffer(leaf, subvol_info->name,
					   item_off, item_len);
		} else {
			ret = -ENOENT;
			goto out;
		}
	}

	if (copy_to_user(argp, subvol_info, sizeof(*subvol_info)))
		ret = -EFAULT;

out:
	btrfs_free_path(path);
	kzfree(subvol_info);
	return ret;
}