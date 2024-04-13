static void mptsas_free_request(MPTSASRequest *req)
{
    if (req->sreq != NULL) {
        req->sreq->hba_private = NULL;
        scsi_req_unref(req->sreq);
        req->sreq = NULL;
    }
    qemu_sglist_destroy(&req->qsg);
    g_free(req);
}