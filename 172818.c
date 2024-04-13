int ext4_issue_zeroout(struct inode *inode, ext4_lblk_t lblk, ext4_fsblk_t pblk,
		       ext4_lblk_t len)
{
	int ret;

	if (IS_ENCRYPTED(inode))
		return fscrypt_zeroout_range(inode, lblk, pblk, len);

	ret = sb_issue_zeroout(inode->i_sb, pblk, len, GFP_NOFS);
	if (ret > 0)
		ret = 0;

	return ret;
}