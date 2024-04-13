void fatal(const char* s, int code)
{
    static int recurse_code = 0;
    const char *fatal = "Fatal error: ";

    if (recurse_code) {
        /* We were called recursively. Just give up */
        proc_cleanup();
        exit(recurse_code);
    }
    recurse_code = code;
    if (httpd_out) {
        prot_printf(httpd_out,
                    "HTTP/1.1 %s\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: %zu\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "%s%s\r\n",
                    error_message(HTTP_SERVER_ERROR),
                    strlen(fatal) + strlen(s) + 2, fatal, s);
        prot_flush(httpd_out);
    }
    syslog(LOG_ERR, "%s%s", fatal, s);
    shut_down(code);
}