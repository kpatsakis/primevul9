static int mwifiex_prog_fw_w_helper(struct mwifiex_adapter *adapter,
				    struct mwifiex_fw_image *fw)
{
	int ret;
	u8 *firmware = fw->fw_buf;
	u32 firmware_len = fw->fw_len;
	u32 offset = 0;
	struct sk_buff *skb;
	u32 txlen, tx_blocks = 0, tries, len, val;
	u32 block_retry_cnt = 0;
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	if (!firmware || !firmware_len) {
		mwifiex_dbg(adapter, ERROR,
			    "No firmware image found! Terminating download\n");
		return -1;
	}

	mwifiex_dbg(adapter, INFO,
		    "info: Downloading FW image (%d bytes)\n",
		    firmware_len);

	if (mwifiex_pcie_disable_host_int(adapter)) {
		mwifiex_dbg(adapter, ERROR,
			    "%s: Disabling interrupts failed.\n", __func__);
		return -1;
	}

	skb = dev_alloc_skb(MWIFIEX_UPLD_SIZE);
	if (!skb) {
		ret = -ENOMEM;
		goto done;
	}

	ret = mwifiex_read_reg(adapter, PCIE_SCRATCH_13_REG, &val);
	if (ret) {
		mwifiex_dbg(adapter, FATAL, "Failed to read scratch register 13\n");
		goto done;
	}

	/* PCIE FLR case: extract wifi part from combo firmware*/
	if (val == MWIFIEX_PCIE_FLR_HAPPENS) {
		ret = mwifiex_extract_wifi_fw(adapter, firmware, firmware_len);
		if (ret < 0) {
			mwifiex_dbg(adapter, ERROR, "Failed to extract wifi fw\n");
			goto done;
		}
		offset = ret;
		mwifiex_dbg(adapter, MSG,
			    "info: dnld wifi firmware from %d bytes\n", offset);
	}

	/* Perform firmware data transfer */
	do {
		u32 ireg_intr = 0;

		/* More data? */
		if (offset >= firmware_len)
			break;

		for (tries = 0; tries < MAX_POLL_TRIES; tries++) {
			ret = mwifiex_read_reg(adapter, reg->cmd_size,
					       &len);
			if (ret) {
				mwifiex_dbg(adapter, FATAL,
					    "Failed reading len from boot code\n");
				goto done;
			}
			if (len)
				break;
			usleep_range(10, 20);
		}

		if (!len) {
			break;
		} else if (len > MWIFIEX_UPLD_SIZE) {
			mwifiex_dbg(adapter, ERROR,
				    "FW download failure @ %d, invalid length %d\n",
				    offset, len);
			ret = -1;
			goto done;
		}

		txlen = len;

		if (len & BIT(0)) {
			block_retry_cnt++;
			if (block_retry_cnt > MAX_WRITE_IOMEM_RETRY) {
				mwifiex_dbg(adapter, ERROR,
					    "FW download failure @ %d, over max\t"
					    "retry count\n", offset);
				ret = -1;
				goto done;
			}
			mwifiex_dbg(adapter, ERROR,
				    "FW CRC error indicated by the\t"
				    "helper: len = 0x%04X, txlen = %d\n",
				    len, txlen);
			len &= ~BIT(0);
			/* Setting this to 0 to resend from same offset */
			txlen = 0;
		} else {
			block_retry_cnt = 0;
			/* Set blocksize to transfer - checking for
			   last block */
			if (firmware_len - offset < txlen)
				txlen = firmware_len - offset;

			tx_blocks = (txlen + card->pcie.blksz_fw_dl - 1) /
				    card->pcie.blksz_fw_dl;

			/* Copy payload to buffer */
			memmove(skb->data, &firmware[offset], txlen);
		}

		skb_put(skb, MWIFIEX_UPLD_SIZE - skb->len);
		skb_trim(skb, tx_blocks * card->pcie.blksz_fw_dl);

		/* Send the boot command to device */
		if (mwifiex_pcie_send_boot_cmd(adapter, skb)) {
			mwifiex_dbg(adapter, ERROR,
				    "Failed to send firmware download command\n");
			ret = -1;
			goto done;
		}

		/* Wait for the command done interrupt */
		for (tries = 0; tries < MAX_POLL_TRIES; tries++) {
			if (mwifiex_read_reg(adapter, PCIE_CPU_INT_STATUS,
					     &ireg_intr)) {
				mwifiex_dbg(adapter, ERROR,
					    "%s: Failed to read\t"
					    "interrupt status during fw dnld.\n",
					    __func__);
				mwifiex_unmap_pci_memory(adapter, skb,
							 PCI_DMA_TODEVICE);
				ret = -1;
				goto done;
			}
			if (!(ireg_intr & CPU_INTR_DOOR_BELL))
				break;
			usleep_range(10, 20);
		}
		if (ireg_intr & CPU_INTR_DOOR_BELL) {
			mwifiex_dbg(adapter, ERROR, "%s: Card failed to ACK download\n",
				    __func__);
			mwifiex_unmap_pci_memory(adapter, skb,
						 PCI_DMA_TODEVICE);
			ret = -1;
			goto done;
		}

		mwifiex_unmap_pci_memory(adapter, skb, PCI_DMA_TODEVICE);

		offset += txlen;
	} while (true);

	mwifiex_dbg(adapter, MSG,
		    "info: FW download over, size %d bytes\n", offset);

	ret = 0;

done:
	dev_kfree_skb_any(skb);
	return ret;
}