static u32 bpf_map_flags_retain_permanent(u32 flags)
{
	/* Some map creation flags are not tied to the map object but
	 * rather to the map fd instead, so they have no meaning upon
	 * map object inspection since multiple file descriptors with
	 * different (access) properties can exist here. Thus, given
	 * this has zero meaning for the map itself, lets clear these
	 * from here.
	 */
	return flags & ~(BPF_F_RDONLY | BPF_F_WRONLY);
}