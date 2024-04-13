mwifiex_wmm_compute_drv_pkt_delay(struct mwifiex_private *priv,
				  const struct sk_buff *skb)
{
	u32 queue_delay = ktime_to_ms(net_timedelta(skb->tstamp));
	u8 ret_val;

	/*
	 * Queue delay is passed as a uint8 in units of 2ms (ms shifted
	 *  by 1). Min value (other than 0) is therefore 2ms, max is 510ms.
	 *
	 * Pass max value if queue_delay is beyond the uint8 range
	 */
	ret_val = (u8) (min(queue_delay, priv->wmm.drv_pkt_delay_max) >> 1);

	mwifiex_dbg(priv->adapter, DATA, "data: WMM: Pkt Delay: %d ms,\t"
		    "%d ms sent to FW\n", queue_delay, ret_val);

	return ret_val;
}