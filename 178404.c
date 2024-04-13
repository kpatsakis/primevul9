disable_regexp_timeout(void)
{
    stop_timeout();
    timeout_flag = &dummy_timeout_flag;
}