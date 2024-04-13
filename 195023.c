static uint16_t rtl8139_TSAD_read(RTL8139State *s)
{
    uint16_t ret = 0;

    /* Simulate TSAD, it is read only anyway */

    ret = ((s->TxStatus[3] & TxStatOK  )?TSAD_TOK3:0)
         |((s->TxStatus[2] & TxStatOK  )?TSAD_TOK2:0)
         |((s->TxStatus[1] & TxStatOK  )?TSAD_TOK1:0)
         |((s->TxStatus[0] & TxStatOK  )?TSAD_TOK0:0)

         |((s->TxStatus[3] & TxUnderrun)?TSAD_TUN3:0)
         |((s->TxStatus[2] & TxUnderrun)?TSAD_TUN2:0)
         |((s->TxStatus[1] & TxUnderrun)?TSAD_TUN1:0)
         |((s->TxStatus[0] & TxUnderrun)?TSAD_TUN0:0)

         |((s->TxStatus[3] & TxAborted )?TSAD_TABT3:0)
         |((s->TxStatus[2] & TxAborted )?TSAD_TABT2:0)
         |((s->TxStatus[1] & TxAborted )?TSAD_TABT1:0)
         |((s->TxStatus[0] & TxAborted )?TSAD_TABT0:0)

         |((s->TxStatus[3] & TxHostOwns )?TSAD_OWN3:0)
         |((s->TxStatus[2] & TxHostOwns )?TSAD_OWN2:0)
         |((s->TxStatus[1] & TxHostOwns )?TSAD_OWN1:0)
         |((s->TxStatus[0] & TxHostOwns )?TSAD_OWN0:0) ;


    DPRINTF("TSAD read val=0x%04x\n", ret);

    return ret;
}