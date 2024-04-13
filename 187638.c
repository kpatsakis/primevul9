void usage(void)
{
    prot_printf(httpd_out, "%s: usage: httpd [-C <alt_config>] [-s]\r\n",
                error_message(HTTP_SERVER_ERROR));
    prot_flush(httpd_out);
    exit(EX_USAGE);
}