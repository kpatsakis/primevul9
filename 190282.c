static php_stream_filter_status_t php_iconv_stream_filter_do_filter(
		php_stream *stream, php_stream_filter *filter,
		php_stream_bucket_brigade *buckets_in,
		php_stream_bucket_brigade *buckets_out,
		size_t *bytes_consumed, int flags TSRMLS_DC)
{
	php_stream_bucket *bucket = NULL;
	size_t consumed = 0;
	php_iconv_stream_filter *self = (php_iconv_stream_filter *)filter->abstract;

	while (buckets_in->head != NULL) {
		bucket = buckets_in->head;

		php_stream_bucket_unlink(bucket TSRMLS_CC);

		if (php_iconv_stream_filter_append_bucket(self, stream, filter,
				buckets_out, bucket->buf, bucket->buflen, &consumed,
				php_stream_is_persistent(stream) TSRMLS_CC) != SUCCESS) {
			goto out_failure;
		}

		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags != PSFS_FLAG_NORMAL) {
		if (php_iconv_stream_filter_append_bucket(self, stream, filter,
				buckets_out, NULL, 0, &consumed,
				php_stream_is_persistent(stream) TSRMLS_CC) != SUCCESS) {
			goto out_failure;
		}
	}

	if (bytes_consumed != NULL) {
		*bytes_consumed = consumed;
	}

	return PSFS_PASS_ON;

out_failure:
	if (bucket != NULL) {
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}
	return PSFS_ERR_FATAL;
}