void iov_iter_xarray(struct iov_iter *i, unsigned int direction,
		     struct xarray *xarray, loff_t start, size_t count)
{
	BUG_ON(direction & ~1);
	*i = (struct iov_iter) {
		.iter_type = ITER_XARRAY,
		.data_source = direction,
		.xarray = xarray,
		.xarray_start = start,
		.count = count,
		.iov_offset = 0
	};
}