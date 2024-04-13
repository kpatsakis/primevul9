isdn_command(isdn_ctrl *cmd)
{
	if (cmd->driver == -1) {
		printk(KERN_WARNING "isdn_command command(%x) driver -1\n", cmd->command);
		return (1);
	}
	if (!dev->drv[cmd->driver]) {
		printk(KERN_WARNING "isdn_command command(%x) dev->drv[%d] NULL\n",
		       cmd->command, cmd->driver);
		return (1);
	}
	if (!dev->drv[cmd->driver]->interface) {
		printk(KERN_WARNING "isdn_command command(%x) dev->drv[%d]->interface NULL\n",
		       cmd->command, cmd->driver);
		return (1);
	}
	if (cmd->command == ISDN_CMD_SETL2) {
		int idx = isdn_dc2minor(cmd->driver, cmd->arg & 255);
		unsigned long l2prot = (cmd->arg >> 8) & 255;
		unsigned long features = (dev->drv[cmd->driver]->interface->features
					  >> ISDN_FEATURE_L2_SHIFT) &
			ISDN_FEATURE_L2_MASK;
		unsigned long l2_feature = (1 << l2prot);

		switch (l2prot) {
		case ISDN_PROTO_L2_V11096:
		case ISDN_PROTO_L2_V11019:
		case ISDN_PROTO_L2_V11038:
			/* If V.110 requested, but not supported by
			 * HL-driver, set emulator-flag and change
			 * Layer-2 to transparent
			 */
			if (!(features & l2_feature)) {
				dev->v110emu[idx] = l2prot;
				cmd->arg = (cmd->arg & 255) |
					(ISDN_PROTO_L2_TRANS << 8);
			} else
				dev->v110emu[idx] = 0;
		}
	}
	return dev->drv[cmd->driver]->interface->command(cmd);
}