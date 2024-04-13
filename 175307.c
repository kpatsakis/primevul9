static void skb_panic(struct sk_buff *skb, unsigned int sz, void *addr,
		      const char msg[])
{
	pr_emerg("%s: text:%p len:%d put:%d head:%p data:%p tail:%#lx end:%#lx dev:%s\n",
		 msg, addr, skb->len, sz, skb->head, skb->data,
		 (unsigned long)skb->tail, (unsigned long)skb->end,
		 skb->dev ? skb->dev->name : "<NULL>");
	BUG();
}