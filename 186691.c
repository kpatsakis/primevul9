static void mptsas_save_request(QEMUFile *f, SCSIRequest *sreq)
{
    MPTSASRequest *req = sreq->hba_private;
    int i;

    qemu_put_buffer(f, (unsigned char *)&req->scsi_io, sizeof(req->scsi_io));
    qemu_put_be32(f, req->qsg.nsg);
    for (i = 0; i < req->qsg.nsg; i++) {
        qemu_put_be64(f, req->qsg.sg[i].base);
        qemu_put_be64(f, req->qsg.sg[i].len);
    }
}