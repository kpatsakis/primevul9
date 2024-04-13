static void f2fs_dio_end_io(struct bio *bio)
{
	struct f2fs_private_dio *dio = bio->bi_private;

	dec_page_count(F2FS_I_SB(dio->inode),
			dio->write ? F2FS_DIO_WRITE : F2FS_DIO_READ);

	bio->bi_private = dio->orig_private;
	bio->bi_end_io = dio->orig_end_io;

	kvfree(dio);

	bio_endio(bio);
}