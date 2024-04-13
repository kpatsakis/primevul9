static int read_raw_super_block(struct super_block *sb,
			struct f2fs_super_block **raw_super,
			struct buffer_head **raw_super_buf,
			int *recovery)
{
	int block = 0;
	struct buffer_head *buffer;
	struct f2fs_super_block *super;
	int err = 0;

retry:
	buffer = sb_bread(sb, block);
	if (!buffer) {
		*recovery = 1;
		f2fs_msg(sb, KERN_ERR, "Unable to read %dth superblock",
				block + 1);
		if (block == 0) {
			block++;
			goto retry;
		} else {
			err = -EIO;
			goto out;
		}
	}

	super = (struct f2fs_super_block *)
		((char *)(buffer)->b_data + F2FS_SUPER_OFFSET);

	/* sanity checking of raw super */
	if (sanity_check_raw_super(sb, super)) {
		brelse(buffer);
		*recovery = 1;
		f2fs_msg(sb, KERN_ERR,
			"Can't find valid F2FS filesystem in %dth superblock",
								block + 1);
		if (block == 0) {
			block++;
			goto retry;
		} else {
			err = -EINVAL;
			goto out;
		}
	}

	if (!*raw_super) {
		*raw_super_buf = buffer;
		*raw_super = super;
	} else {
		/* already have a valid superblock */
		brelse(buffer);
	}

	/* check the validity of the second superblock */
	if (block == 0) {
		block++;
		goto retry;
	}

out:
	/* No valid superblock */
	if (!*raw_super)
		return err;

	return 0;
}