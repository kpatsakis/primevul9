static int comp_keys(const struct btrfs_disk_key *disk_key,
		     const struct btrfs_key *k2)
{
	const struct btrfs_key *k1 = (const struct btrfs_key *)disk_key;

	return btrfs_comp_cpu_keys(k1, k2);
}