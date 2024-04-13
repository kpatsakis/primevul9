
static void ql_get_sbuf(struct ql3_adapter *qdev)
{
	if (++qdev->small_buf_index == NUM_SMALL_BUFFERS)
		qdev->small_buf_index = 0;
	qdev->small_buf_release_cnt++;