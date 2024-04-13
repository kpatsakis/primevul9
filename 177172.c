int ext4_xattr_delete_inode(handle_t *handle, struct inode *inode,
			    struct ext4_xattr_inode_array **ea_inode_array,
			    int extra_credits)
{
	struct buffer_head *bh = NULL;
	struct ext4_xattr_ibody_header *header;
	struct ext4_iloc iloc = { .bh = NULL };
	struct ext4_xattr_entry *entry;
	struct inode *ea_inode;
	int error;

	error = ext4_xattr_ensure_credits(handle, inode, extra_credits,
					  NULL /* bh */,
					  false /* dirty */,
					  false /* block_csum */);
	if (error) {
		EXT4_ERROR_INODE(inode, "ensure credits (error %d)", error);
		goto cleanup;
	}

	if (ext4_has_feature_ea_inode(inode->i_sb) &&
	    ext4_test_inode_state(inode, EXT4_STATE_XATTR)) {

		error = ext4_get_inode_loc(inode, &iloc);
		if (error) {
			EXT4_ERROR_INODE(inode, "inode loc (error %d)", error);
			goto cleanup;
		}

		error = ext4_journal_get_write_access(handle, iloc.bh);
		if (error) {
			EXT4_ERROR_INODE(inode, "write access (error %d)",
					 error);
			goto cleanup;
		}

		header = IHDR(inode, ext4_raw_inode(&iloc));
		if (header->h_magic == cpu_to_le32(EXT4_XATTR_MAGIC))
			ext4_xattr_inode_dec_ref_all(handle, inode, iloc.bh,
						     IFIRST(header),
						     false /* block_csum */,
						     ea_inode_array,
						     extra_credits,
						     false /* skip_quota */);
	}

	if (EXT4_I(inode)->i_file_acl) {
		bh = sb_bread(inode->i_sb, EXT4_I(inode)->i_file_acl);
		if (!bh) {
			EXT4_ERROR_INODE(inode, "block %llu read error",
					 EXT4_I(inode)->i_file_acl);
			error = -EIO;
			goto cleanup;
		}
		error = ext4_xattr_check_block(inode, bh);
		if (error)
			goto cleanup;

		if (ext4_has_feature_ea_inode(inode->i_sb)) {
			for (entry = BFIRST(bh); !IS_LAST_ENTRY(entry);
			     entry = EXT4_XATTR_NEXT(entry)) {
				if (!entry->e_value_inum)
					continue;
				error = ext4_xattr_inode_iget(inode,
					      le32_to_cpu(entry->e_value_inum),
					      le32_to_cpu(entry->e_hash),
					      &ea_inode);
				if (error)
					continue;
				ext4_xattr_inode_free_quota(inode, ea_inode,
					      le32_to_cpu(entry->e_value_size));
				iput(ea_inode);
			}

		}

		ext4_xattr_release_block(handle, inode, bh, ea_inode_array,
					 extra_credits);
		/*
		 * Update i_file_acl value in the same transaction that releases
		 * block.
		 */
		EXT4_I(inode)->i_file_acl = 0;
		error = ext4_mark_inode_dirty(handle, inode);
		if (error) {
			EXT4_ERROR_INODE(inode, "mark inode dirty (error %d)",
					 error);
			goto cleanup;
		}
	}
	error = 0;
cleanup:
	brelse(iloc.bh);
	brelse(bh);
	return error;
}