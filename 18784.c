termrequest_sent(termrequest_T *status)
{
    status->tr_progress = STATUS_SENT;
    status->tr_start = time(NULL);
}