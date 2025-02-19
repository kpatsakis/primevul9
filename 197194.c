process_tx_desc(E1000State *s, struct e1000_tx_desc *dp)
{
    uint32_t txd_lower = le32_to_cpu(dp->lower.data);
    uint32_t dtype = txd_lower & (E1000_TXD_CMD_DEXT | E1000_TXD_DTYP_D);
    unsigned int split_size = txd_lower & 0xffff, bytes, sz, op;
    unsigned int msh = 0xfffff, hdr = 0;
    uint64_t addr;
    struct e1000_context_desc *xp = (struct e1000_context_desc *)dp;
    struct e1000_tx *tp = &s->tx;

    if (dtype == E1000_TXD_CMD_DEXT) {	// context descriptor
        op = le32_to_cpu(xp->cmd_and_length);
        tp->ipcss = xp->lower_setup.ip_fields.ipcss;
        tp->ipcso = xp->lower_setup.ip_fields.ipcso;
        tp->ipcse = le16_to_cpu(xp->lower_setup.ip_fields.ipcse);
        tp->tucss = xp->upper_setup.tcp_fields.tucss;
        tp->tucso = xp->upper_setup.tcp_fields.tucso;
        tp->tucse = le16_to_cpu(xp->upper_setup.tcp_fields.tucse);
        tp->paylen = op & 0xfffff;
        tp->hdr_len = xp->tcp_seg_setup.fields.hdr_len;
        tp->mss = le16_to_cpu(xp->tcp_seg_setup.fields.mss);
        tp->ip = (op & E1000_TXD_CMD_IP) ? 1 : 0;
        tp->tcp = (op & E1000_TXD_CMD_TCP) ? 1 : 0;
        tp->tse = (op & E1000_TXD_CMD_TSE) ? 1 : 0;
        tp->tso_frames = 0;
        if (tp->tucso == 0) {	// this is probably wrong
            DBGOUT(TXSUM, "TCP/UDP: cso 0!\n");
            tp->tucso = tp->tucss + (tp->tcp ? 16 : 6);
        }
        return;
    } else if (dtype == (E1000_TXD_CMD_DEXT | E1000_TXD_DTYP_D)) {
        // data descriptor
        if (tp->size == 0) {
            tp->sum_needed = le32_to_cpu(dp->upper.data) >> 8;
        }
        tp->cptse = ( txd_lower & E1000_TXD_CMD_TSE ) ? 1 : 0;
    } else {
        // legacy descriptor
        tp->cptse = 0;
    }

    if (vlan_enabled(s) && is_vlan_txd(txd_lower) &&
        (tp->cptse || txd_lower & E1000_TXD_CMD_EOP)) {
        tp->vlan_needed = 1;
        cpu_to_be16wu((uint16_t *)(tp->vlan_header),
                      le16_to_cpup((uint16_t *)(s->mac_reg + VET)));
        cpu_to_be16wu((uint16_t *)(tp->vlan_header + 2),
                      le16_to_cpu(dp->upper.fields.special));
    }
        
    addr = le64_to_cpu(dp->buffer_addr);
    if (tp->tse && tp->cptse) {
        hdr = tp->hdr_len;
        msh = hdr + tp->mss;
        do {
            bytes = split_size;
            if (tp->size + bytes > msh)
                bytes = msh - tp->size;

            bytes = MIN(sizeof(tp->data) - tp->size, bytes);
            pci_dma_read(&s->dev, addr, tp->data + tp->size, bytes);
            if ((sz = tp->size + bytes) >= hdr && tp->size < hdr)
                memmove(tp->header, tp->data, hdr);
            tp->size = sz;
            addr += bytes;
            if (sz == msh) {
                xmit_seg(s);
                memmove(tp->data, tp->header, hdr);
                tp->size = hdr;
            }
        } while (split_size -= bytes);
    } else if (!tp->tse && tp->cptse) {
        // context descriptor TSE is not set, while data descriptor TSE is set
        DBGOUT(TXERR, "TCP segmentation error\n");
    } else {
        split_size = MIN(sizeof(tp->data) - tp->size, split_size);
        pci_dma_read(&s->dev, addr, tp->data + tp->size, split_size);
        tp->size += split_size;
    }

    if (!(txd_lower & E1000_TXD_CMD_EOP))
        return;
    if (!(tp->tse && tp->cptse && tp->size < hdr))
        xmit_seg(s);
    tp->tso_frames = 0;
    tp->sum_needed = 0;
    tp->vlan_needed = 0;
    tp->size = 0;
    tp->cptse = 0;
}