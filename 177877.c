data_sock_ioctl_bound(struct sock *sk, unsigned int cmd, void __user *p)
{
	struct mISDN_ctrl_req	cq;
	int			err = -EINVAL, val[2];
	struct mISDNchannel	*bchan, *next;

	lock_sock(sk);
	if (!_pms(sk)->dev) {
		err = -ENODEV;
		goto done;
	}
	switch (cmd) {
	case IMCTRLREQ:
		if (copy_from_user(&cq, p, sizeof(cq))) {
			err = -EFAULT;
			break;
		}
		if ((sk->sk_protocol & ~ISDN_P_B_MASK) == ISDN_P_B_START) {
			list_for_each_entry_safe(bchan, next,
						 &_pms(sk)->dev->bchannels, list) {
				if (bchan->nr == cq.channel) {
					err = bchan->ctrl(bchan,
							  CONTROL_CHANNEL, &cq);
					break;
				}
			}
		} else
			err = _pms(sk)->dev->D.ctrl(&_pms(sk)->dev->D,
						    CONTROL_CHANNEL, &cq);
		if (err)
			break;
		if (copy_to_user(p, &cq, sizeof(cq)))
			err = -EFAULT;
		break;
	case IMCLEAR_L2:
		if (sk->sk_protocol != ISDN_P_LAPD_NT) {
			err = -EINVAL;
			break;
		}
		val[0] = cmd;
		if (get_user(val[1], (int __user *)p)) {
			err = -EFAULT;
			break;
		}
		err = _pms(sk)->dev->teimgr->ctrl(_pms(sk)->dev->teimgr,
						  CONTROL_CHANNEL, val);
		break;
	case IMHOLD_L1:
		if (sk->sk_protocol != ISDN_P_LAPD_NT
		    && sk->sk_protocol != ISDN_P_LAPD_TE) {
			err = -EINVAL;
			break;
		}
		val[0] = cmd;
		if (get_user(val[1], (int __user *)p)) {
			err = -EFAULT;
			break;
		}
		err = _pms(sk)->dev->teimgr->ctrl(_pms(sk)->dev->teimgr,
						  CONTROL_CHANNEL, val);
		break;
	default:
		err = -EINVAL;
		break;
	}
done:
	release_sock(sk);
	return err;
}