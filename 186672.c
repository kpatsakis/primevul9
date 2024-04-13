static void mptsas_cancel_notify(Notifier *notifier, void *data)
{
    MPTSASCancelNotifier *n = container_of(notifier,
                                           MPTSASCancelNotifier,
                                           notifier);

    /* Abusing IOCLogInfo to store the expected number of requests... */
    if (++n->reply->TerminationCount == n->reply->IOCLogInfo) {
        n->reply->IOCLogInfo = 0;
        mptsas_fix_scsi_task_mgmt_reply_endianness(n->reply);
        mptsas_post_reply(n->s, (MPIDefaultReply *)n->reply);
        g_free(n->reply);
    }
    g_free(n);
}