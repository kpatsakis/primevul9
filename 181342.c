static void v9fs_init_qiov_from_pdu(QEMUIOVector *qiov, V9fsPDU *pdu,
                                    size_t skip, size_t size,
                                    bool is_write)
{
    QEMUIOVector elem;
    struct iovec *iov;
    unsigned int niov;

    if (is_write) {
        pdu->s->transport->init_out_iov_from_pdu(pdu, &iov, &niov, size + skip);
    } else {
        pdu->s->transport->init_in_iov_from_pdu(pdu, &iov, &niov, size + skip);
    }

    qemu_iovec_init_external(&elem, iov, niov);
    qemu_iovec_init(qiov, niov);
    qemu_iovec_concat(qiov, &elem, skip, size);
}