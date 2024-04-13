static void ll_temac_recv(struct net_device *ndev)
{
	struct temac_local *lp = netdev_priv(ndev);
	unsigned long flags;
	int rx_bd;
	bool update_tail = false;

	spin_lock_irqsave(&lp->rx_lock, flags);

	/* Process all received buffers, passing them on network
	 * stack.  After this, the buffer descriptors will be in an
	 * un-allocated stage, where no skb is allocated for it, and
	 * they are therefore not available for TEMAC/DMA.
	 */
	do {
		struct cdmac_bd *bd = &lp->rx_bd_v[lp->rx_bd_ci];
		struct sk_buff *skb = lp->rx_skb[lp->rx_bd_ci];
		unsigned int bdstat = be32_to_cpu(bd->app0);
		int length;

		/* While this should not normally happen, we can end
		 * here when GFP_ATOMIC allocations fail, and we
		 * therefore have un-allocated buffers.
		 */
		if (!skb)
			break;

		/* Loop over all completed buffer descriptors */
		if (!(bdstat & STS_CTRL_APP0_CMPLT))
			break;

		dma_unmap_single(ndev->dev.parent, be32_to_cpu(bd->phys),
				 XTE_MAX_JUMBO_FRAME_SIZE, DMA_FROM_DEVICE);
		/* The buffer is not valid for DMA anymore */
		bd->phys = 0;
		bd->len = 0;

		length = be32_to_cpu(bd->app4) & 0x3FFF;
		skb_put(skb, length);
		skb->protocol = eth_type_trans(skb, ndev);
		skb_checksum_none_assert(skb);

		/* if we're doing rx csum offload, set it up */
		if (((lp->temac_features & TEMAC_FEATURE_RX_CSUM) != 0) &&
		    (skb->protocol == htons(ETH_P_IP)) &&
		    (skb->len > 64)) {

			/* Convert from device endianness (be32) to cpu
			 * endiannes, and if necessary swap the bytes
			 * (back) for proper IP checksum byte order
			 * (be16).
			 */
			skb->csum = htons(be32_to_cpu(bd->app3) & 0xFFFF);
			skb->ip_summed = CHECKSUM_COMPLETE;
		}

		if (!skb_defer_rx_timestamp(skb))
			netif_rx(skb);
		/* The skb buffer is now owned by network stack above */
		lp->rx_skb[lp->rx_bd_ci] = NULL;

		ndev->stats.rx_packets++;
		ndev->stats.rx_bytes += length;

		rx_bd = lp->rx_bd_ci;
		if (++lp->rx_bd_ci >= lp->rx_bd_num)
			lp->rx_bd_ci = 0;
	} while (rx_bd != lp->rx_bd_tail);

	/* DMA operations will halt when the last buffer descriptor is
	 * processed (ie. the one pointed to by RX_TAILDESC_PTR).
	 * When that happens, no more interrupt events will be
	 * generated.  No IRQ_COAL or IRQ_DLY, and not even an
	 * IRQ_ERR.  To avoid stalling, we schedule a delayed work
	 * when there is a potential risk of that happening.  The work
	 * will call this function, and thus re-schedule itself until
	 * enough buffers are available again.
	 */
	if (ll_temac_recv_buffers_available(lp) < lp->coalesce_count_rx)
		schedule_delayed_work(&lp->restart_work, HZ / 1000);

	/* Allocate new buffers for those buffer descriptors that were
	 * passed to network stack.  Note that GFP_ATOMIC allocations
	 * can fail (e.g. when a larger burst of GFP_ATOMIC
	 * allocations occurs), so while we try to allocate all
	 * buffers in the same interrupt where they were processed, we
	 * continue with what we could get in case of allocation
	 * failure.  Allocation of remaining buffers will be retried
	 * in following calls.
	 */
	while (1) {
		struct sk_buff *skb;
		struct cdmac_bd *bd;
		dma_addr_t skb_dma_addr;

		rx_bd = lp->rx_bd_tail + 1;
		if (rx_bd >= lp->rx_bd_num)
			rx_bd = 0;
		bd = &lp->rx_bd_v[rx_bd];

		if (bd->phys)
			break;	/* All skb's allocated */

		skb = netdev_alloc_skb_ip_align(ndev, XTE_MAX_JUMBO_FRAME_SIZE);
		if (!skb) {
			dev_warn(&ndev->dev, "skb alloc failed\n");
			break;
		}

		skb_dma_addr = dma_map_single(ndev->dev.parent, skb->data,
					      XTE_MAX_JUMBO_FRAME_SIZE,
					      DMA_FROM_DEVICE);
		if (WARN_ON_ONCE(dma_mapping_error(ndev->dev.parent,
						   skb_dma_addr))) {
			dev_kfree_skb_any(skb);
			break;
		}

		bd->phys = cpu_to_be32(skb_dma_addr);
		bd->len = cpu_to_be32(XTE_MAX_JUMBO_FRAME_SIZE);
		bd->app0 = cpu_to_be32(STS_CTRL_APP0_IRQONEND);
		lp->rx_skb[rx_bd] = skb;

		lp->rx_bd_tail = rx_bd;
		update_tail = true;
	}

	/* Move tail pointer when buffers have been allocated */
	if (update_tail) {
		lp->dma_out(lp, RX_TAILDESC_PTR,
			lp->rx_bd_p + sizeof(*lp->rx_bd_v) * lp->rx_bd_tail);
	}

	spin_unlock_irqrestore(&lp->rx_lock, flags);
}