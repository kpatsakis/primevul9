vmxnet3_tx_retrieve_metadata(VMXNET3State *s,
                             const struct Vmxnet3_TxDesc *txd)
{
    s->offload_mode = txd->om;
    s->cso_or_gso_size = txd->msscof;
    s->tci = txd->tci;
    s->needs_vlan = txd->ti;
}