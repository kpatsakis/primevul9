mwifiex_wmm_add_buf_bypass_txqueue(struct mwifiex_private *priv,
				   struct sk_buff *skb)
{
	skb_queue_tail(&priv->bypass_txq, skb);
}