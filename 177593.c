isdn_net_dial(void)
{
	isdn_net_dev *p = dev->netdev;
	int anymore = 0;
	int i;
	isdn_ctrl cmd;
	u_char *phone_number;

	while (p) {
		isdn_net_local *lp = p->local;

#ifdef ISDN_DEBUG_NET_DIAL
		if (lp->dialstate)
			printk(KERN_DEBUG "%s: dialstate=%d\n", p->dev->name, lp->dialstate);
#endif
		switch (lp->dialstate) {
		case 0:
			/* Nothing to do for this interface */
			break;
		case 1:
			/* Initiate dialout. Set phone-number-pointer to first number
			 * of interface.
			 */
			lp->dial = lp->phone[1];
			if (!lp->dial) {
				printk(KERN_WARNING "%s: phone number deleted?\n",
				       p->dev->name);
				isdn_net_hangup(p->dev);
				break;
			}
			anymore = 1;

			if (lp->dialtimeout > 0)
				if (lp->dialstarted == 0 || time_after(jiffies, lp->dialstarted + lp->dialtimeout + lp->dialwait)) {
					lp->dialstarted = jiffies;
					lp->dialwait_timer = 0;
				}

			lp->dialstate++;
			/* Fall through */
		case 2:
			/* Prepare dialing. Clear EAZ, then set EAZ. */
			cmd.driver = lp->isdn_device;
			cmd.arg = lp->isdn_channel;
			cmd.command = ISDN_CMD_CLREAZ;
			isdn_command(&cmd);
			sprintf(cmd.parm.num, "%s", isdn_map_eaz2msn(lp->msn, cmd.driver));
			cmd.command = ISDN_CMD_SETEAZ;
			isdn_command(&cmd);
			lp->dialretry = 0;
			anymore = 1;
			lp->dialstate++;
			/* Fall through */
		case 3:
			/* Setup interface, dial current phone-number, switch to next number.
			 * If list of phone-numbers is exhausted, increment
			 * retry-counter.
			 */
			if (dev->global_flags & ISDN_GLOBAL_STOPPED || (ISDN_NET_DIALMODE(*lp) == ISDN_NET_DM_OFF)) {
				char *s;
				if (dev->global_flags & ISDN_GLOBAL_STOPPED)
					s = "dial suppressed: isdn system stopped";
				else
					s = "dial suppressed: dialmode `off'";
				isdn_net_unreachable(p->dev, NULL, s);
				isdn_net_hangup(p->dev);
				break;
			}
			cmd.driver = lp->isdn_device;
			cmd.command = ISDN_CMD_SETL2;
			cmd.arg = lp->isdn_channel + (lp->l2_proto << 8);
			isdn_command(&cmd);
			cmd.driver = lp->isdn_device;
			cmd.command = ISDN_CMD_SETL3;
			cmd.arg = lp->isdn_channel + (lp->l3_proto << 8);
			isdn_command(&cmd);
			cmd.driver = lp->isdn_device;
			cmd.arg = lp->isdn_channel;
			if (!lp->dial) {
				printk(KERN_WARNING "%s: phone number deleted?\n",
				       p->dev->name);
				isdn_net_hangup(p->dev);
				break;
			}
			if (!strncmp(lp->dial->num, "LEASED", strlen("LEASED"))) {
				lp->dialstate = 4;
				printk(KERN_INFO "%s: Open leased line ...\n", p->dev->name);
			} else {
				if (lp->dialtimeout > 0)
					if (time_after(jiffies, lp->dialstarted + lp->dialtimeout)) {
						lp->dialwait_timer = jiffies + lp->dialwait;
						lp->dialstarted = 0;
						isdn_net_unreachable(p->dev, NULL, "dial: timed out");
						isdn_net_hangup(p->dev);
						break;
					}

				cmd.driver = lp->isdn_device;
				cmd.command = ISDN_CMD_DIAL;
				cmd.parm.setup.si2 = 0;

				/* check for DOV */
				phone_number = lp->dial->num;
				if ((*phone_number == 'v') ||
				    (*phone_number == 'V')) { /* DOV call */
					cmd.parm.setup.si1 = 1;
				} else { /* DATA call */
					cmd.parm.setup.si1 = 7;
				}

				strcpy(cmd.parm.setup.phone, phone_number);
				/*
				 * Switch to next number or back to start if at end of list.
				 */
				if (!(lp->dial = (isdn_net_phone *) lp->dial->next)) {
					lp->dial = lp->phone[1];
					lp->dialretry++;

					if (lp->dialretry > lp->dialmax) {
						if (lp->dialtimeout == 0) {
							lp->dialwait_timer = jiffies + lp->dialwait;
							lp->dialstarted = 0;
							isdn_net_unreachable(p->dev, NULL, "dial: tried all numbers dialmax times");
						}
						isdn_net_hangup(p->dev);
						break;
					}
				}
				sprintf(cmd.parm.setup.eazmsn, "%s",
					isdn_map_eaz2msn(lp->msn, cmd.driver));
				i = isdn_dc2minor(lp->isdn_device, lp->isdn_channel);
				if (i >= 0) {
					strcpy(dev->num[i], cmd.parm.setup.phone);
					dev->usage[i] |= ISDN_USAGE_OUTGOING;
					isdn_info_update();
				}
				printk(KERN_INFO "%s: dialing %d %s... %s\n", p->dev->name,
				       lp->dialretry, cmd.parm.setup.phone,
				       (cmd.parm.setup.si1 == 1) ? "DOV" : "");
				lp->dtimer = 0;
#ifdef ISDN_DEBUG_NET_DIAL
				printk(KERN_DEBUG "dial: d=%d c=%d\n", lp->isdn_device,
				       lp->isdn_channel);
#endif
				isdn_command(&cmd);
			}
			lp->huptimer = 0;
			lp->outgoing = 1;
			if (lp->chargeint) {
				lp->hupflags |= ISDN_HAVECHARGE;
				lp->hupflags &= ~ISDN_WAITCHARGE;
			} else {
				lp->hupflags |= ISDN_WAITCHARGE;
				lp->hupflags &= ~ISDN_HAVECHARGE;
			}
			anymore = 1;
			lp->dialstate =
				(lp->cbdelay &&
				 (lp->flags & ISDN_NET_CBOUT)) ? 12 : 4;
			break;
		case 4:
			/* Wait for D-Channel-connect.
			 * If timeout, switch back to state 3.
			 * Dialmax-handling moved to state 3.
			 */
			if (lp->dtimer++ > ISDN_TIMER_DTIMEOUT10)
				lp->dialstate = 3;
			anymore = 1;
			break;
		case 5:
			/* Got D-Channel-Connect, send B-Channel-request */
			cmd.driver = lp->isdn_device;
			cmd.arg = lp->isdn_channel;
			cmd.command = ISDN_CMD_ACCEPTB;
			anymore = 1;
			lp->dtimer = 0;
			lp->dialstate++;
			isdn_command(&cmd);
			break;
		case 6:
			/* Wait for B- or D-Channel-connect. If timeout,
			 * switch back to state 3.
			 */
#ifdef ISDN_DEBUG_NET_DIAL
			printk(KERN_DEBUG "dialtimer2: %d\n", lp->dtimer);
#endif
			if (lp->dtimer++ > ISDN_TIMER_DTIMEOUT10)
				lp->dialstate = 3;
			anymore = 1;
			break;
		case 7:
			/* Got incoming Call, setup L2 and L3 protocols,
			 * then wait for D-Channel-connect
			 */
#ifdef ISDN_DEBUG_NET_DIAL
			printk(KERN_DEBUG "dialtimer4: %d\n", lp->dtimer);
#endif
			cmd.driver = lp->isdn_device;
			cmd.command = ISDN_CMD_SETL2;
			cmd.arg = lp->isdn_channel + (lp->l2_proto << 8);
			isdn_command(&cmd);
			cmd.driver = lp->isdn_device;
			cmd.command = ISDN_CMD_SETL3;
			cmd.arg = lp->isdn_channel + (lp->l3_proto << 8);
			isdn_command(&cmd);
			if (lp->dtimer++ > ISDN_TIMER_DTIMEOUT15)
				isdn_net_hangup(p->dev);
			else {
				anymore = 1;
				lp->dialstate++;
			}
			break;
		case 9:
			/* Got incoming D-Channel-Connect, send B-Channel-request */
			cmd.driver = lp->isdn_device;
			cmd.arg = lp->isdn_channel;
			cmd.command = ISDN_CMD_ACCEPTB;
			isdn_command(&cmd);
			anymore = 1;
			lp->dtimer = 0;
			lp->dialstate++;
			break;
		case 8:
		case 10:
			/*  Wait for B- or D-channel-connect */
#ifdef ISDN_DEBUG_NET_DIAL
			printk(KERN_DEBUG "dialtimer4: %d\n", lp->dtimer);
#endif
			if (lp->dtimer++ > ISDN_TIMER_DTIMEOUT10)
				isdn_net_hangup(p->dev);
			else
				anymore = 1;
			break;
		case 11:
			/* Callback Delay */
			if (lp->dtimer++ > lp->cbdelay)
				lp->dialstate = 1;
			anymore = 1;
			break;
		case 12:
			/* Remote does callback. Hangup after cbdelay, then wait for incoming
			 * call (in state 4).
			 */
			if (lp->dtimer++ > lp->cbdelay)
			{
				printk(KERN_INFO "%s: hangup waiting for callback ...\n", p->dev->name);
				lp->dtimer = 0;
				lp->dialstate = 4;
				cmd.driver = lp->isdn_device;
				cmd.command = ISDN_CMD_HANGUP;
				cmd.arg = lp->isdn_channel;
				isdn_command(&cmd);
				isdn_all_eaz(lp->isdn_device, lp->isdn_channel);
			}
			anymore = 1;
			break;
		default:
			printk(KERN_WARNING "isdn_net: Illegal dialstate %d for device %s\n",
			       lp->dialstate, p->dev->name);
		}
		p = (isdn_net_dev *) p->next;
	}
	isdn_timer_ctrl(ISDN_TIMER_NETDIAL, anymore);
}