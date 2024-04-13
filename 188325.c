void test_checkout_nasty__dotgit_alternate_data_stream(void)
{
	test_checkout_fails("refs/heads/dotgit_alternate_data_stream", ".git/dummy-file");
	test_checkout_fails("refs/heads/dotgit_alternate_data_stream", ".git::$INDEX_ALLOCATION/dummy-file");
}