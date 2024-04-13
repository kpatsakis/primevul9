static void __f2fs_submit_merged_write(struct f2fs_sb_info *sbi,
				enum page_type type, enum temp_type temp)
{
	enum page_type btype = PAGE_TYPE_OF_BIO(type);
	struct f2fs_bio_info *io = sbi->write_io[btype] + temp;

	down_write(&io->io_rwsem);

	/* change META to META_FLUSH in the checkpoint procedure */
	if (type >= META_FLUSH) {
		io->fio.type = META_FLUSH;
		io->fio.op = REQ_OP_WRITE;
		io->fio.op_flags = REQ_META | REQ_PRIO | REQ_SYNC;
		if (!test_opt(sbi, NOBARRIER))
			io->fio.op_flags |= REQ_PREFLUSH | REQ_FUA;
	}
	__submit_merged_bio(io);
	up_write(&io->io_rwsem);
}