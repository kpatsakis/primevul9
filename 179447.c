static inline void set_fs(mm_segment_t fs)
{
	current->thread.addr_limit = fs;
	/* On user-mode return, check fs is correct */
	set_thread_flag(TIF_FSCHECK);
}