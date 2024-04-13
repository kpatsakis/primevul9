	__releases(aarp_lock)
{
	read_unlock_bh(&aarp_lock);
}