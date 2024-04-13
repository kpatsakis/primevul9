static int btrfs_ioctl_get_subvol_rootref(struct file *file, void __user *argp)
{
	struct btrfs_ioctl_get_subvol_rootref_args *rootrefs;
	struct btrfs_root_ref *rref;
	struct btrfs_root *root;
	struct btrfs_path *path;
	struct btrfs_key key;
	struct extent_buffer *leaf;
	struct inode *inode;
	u64 objectid;
	int slot;
	int ret;
	u8 found;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	rootrefs = memdup_user(argp, sizeof(*rootrefs));
	if (IS_ERR(rootrefs)) {
		btrfs_free_path(path);
		return PTR_ERR(rootrefs);
	}

	inode = file_inode(file);
	root = BTRFS_I(inode)->root->fs_info->tree_root;
	objectid = BTRFS_I(inode)->root->root_key.objectid;

	key.objectid = objectid;
	key.type = BTRFS_ROOT_REF_KEY;
	key.offset = rootrefs->min_treeid;
	found = 0;

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
	while (1) {
		leaf = path->nodes[0];
		slot = path->slots[0];

		btrfs_item_key_to_cpu(leaf, &key, slot);
		if (key.objectid != objectid || key.type != BTRFS_ROOT_REF_KEY) {
			ret = 0;
			goto out;
		}

		if (found == BTRFS_MAX_ROOTREF_BUFFER_NUM) {
			ret = -EOVERFLOW;
			goto out;
		}

		rref = btrfs_item_ptr(leaf, slot, struct btrfs_root_ref);
		rootrefs->rootref[found].treeid = key.offset;
		rootrefs->rootref[found].dirid =
				  btrfs_root_ref_dirid(leaf, rref);
		found++;

		ret = btrfs_next_item(root, path);
		if (ret < 0) {
			goto out;
		} else if (ret > 0) {
			ret = -EUCLEAN;
			goto out;
		}
	}

out:
	if (!ret || ret == -EOVERFLOW) {
		rootrefs->num_items = found;
		/* update min_treeid for next search */
		if (found)
			rootrefs->min_treeid =
				rootrefs->rootref[found - 1].treeid + 1;
		if (copy_to_user(argp, rootrefs, sizeof(*rootrefs)))
			ret = -EFAULT;
	}

	kfree(rootrefs);
	btrfs_free_path(path);

	return ret;
}