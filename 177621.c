isdn_net_getphones(isdn_net_ioctl_phone *phone, char __user *phones)
{
	isdn_net_dev *p = isdn_net_findif(phone->name);
	int inout = phone->outgoing & 1;
	int more = 0;
	int count = 0;
	isdn_net_phone *n;

	if (!p)
		return -ENODEV;
	inout &= 1;
	for (n = p->local->phone[inout]; n; n = n->next) {
		if (more) {
			put_user(' ', phones++);
			count++;
		}
		if (copy_to_user(phones, n->num, strlen(n->num) + 1)) {
			return -EFAULT;
		}
		phones += strlen(n->num);
		count += strlen(n->num);
		more = 1;
	}
	put_user(0, phones);
	count++;
	return count;
}