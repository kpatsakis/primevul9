static bool fd_htab_map_needs_adjust(const struct bpf_htab *htab)
{
	return htab->map.map_type == BPF_MAP_TYPE_HASH_OF_MAPS &&
	       BITS_PER_LONG == 64;
}