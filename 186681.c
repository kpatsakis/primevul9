static QEMUSGList *mptsas_get_sg_list(SCSIRequest *sreq)
{
    MPTSASRequest *req = sreq->hba_private;

    return &req->qsg;
}