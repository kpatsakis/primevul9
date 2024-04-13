static void mwifiex_interrupt_status(struct mwifiex_adapter *adapter,
				     int msg_id)
{
	u32 pcie_ireg;
	unsigned long flags;
	struct pcie_service_card *card = adapter->card;

	if (card->msi_enable) {
		spin_lock_irqsave(&adapter->int_lock, flags);
		adapter->int_status = 1;
		spin_unlock_irqrestore(&adapter->int_lock, flags);
		return;
	}

	if (!mwifiex_pcie_ok_to_access_hw(adapter))
		return;

	if (card->msix_enable && msg_id >= 0) {
		pcie_ireg = BIT(msg_id);
	} else {
		if (mwifiex_read_reg(adapter, PCIE_HOST_INT_STATUS,
				     &pcie_ireg)) {
			mwifiex_dbg(adapter, ERROR, "Read register failed\n");
			return;
		}

		if ((pcie_ireg == 0xFFFFFFFF) || !pcie_ireg)
			return;


		mwifiex_pcie_disable_host_int(adapter);

		/* Clear the pending interrupts */
		if (mwifiex_write_reg(adapter, PCIE_HOST_INT_STATUS,
				      ~pcie_ireg)) {
			mwifiex_dbg(adapter, ERROR,
				    "Write register failed\n");
			return;
		}
	}

	if (!adapter->pps_uapsd_mode &&
	    adapter->ps_state == PS_STATE_SLEEP &&
	    mwifiex_pcie_ok_to_access_hw(adapter)) {
		/* Potentially for PCIe we could get other
		 * interrupts like shared. Don't change power
		 * state until cookie is set
		 */
		adapter->ps_state = PS_STATE_AWAKE;
		adapter->pm_wakeup_fw_try = false;
		del_timer(&adapter->wakeup_timer);
	}

	spin_lock_irqsave(&adapter->int_lock, flags);
	adapter->int_status |= pcie_ireg;
	spin_unlock_irqrestore(&adapter->int_lock, flags);
	mwifiex_dbg(adapter, INTR, "ireg: 0x%08x\n", pcie_ireg);
}