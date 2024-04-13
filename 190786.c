static void file_globals_ctor(php_file_globals *file_globals_p TSRMLS_DC)
{
	FG(pclose_ret) = 0;
	FG(pclose_wait) = 0;
	FG(user_stream_current_filename) = NULL;
	FG(def_chunk_size) = PHP_SOCK_CHUNK_SIZE;
	FG(wrapper_errors) = NULL;
}