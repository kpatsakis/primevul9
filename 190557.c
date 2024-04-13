static void f2fs_dio_submit_bio(struct bio *bio, struct inode *inode,
							loff_t file_offset)
{
	struct f2fs_private_dio *dio;
	bool write = (bio_op(bio) == REQ_OP_WRITE);

	dio = f2fs_kzalloc(F2FS_I_SB(inode),
			sizeof(struct f2fs_private_dio), GFP_NOFS);
	if (!dio)
		goto out;

	dio->inode = inode;
	dio->orig_end_io = bio->bi_end_io;
	dio->orig_private = bio->bi_private;
	dio->write = write;

	bio->bi_end_io = f2fs_dio_end_io;
	bio->bi_private = dio;

	inc_page_count(F2FS_I_SB(inode),
			write ? F2FS_DIO_WRITE : F2FS_DIO_READ);

	submit_bio(bio);
	return;
out:
	bio->bi_status = BLK_STS_IOERR;
	bio_endio(bio);
}