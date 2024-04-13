static int pcan_msg_add_rec(struct pcan_usb_pro_msg *pm, int id, ...)
{
	int len, i;
	u8 *pc;
	va_list ap;

	va_start(ap, id);

	pc = pm->rec_ptr + 1;

	i = 0;
	switch (id) {
	case PCAN_USBPRO_TXMSG8:
		i += 4;
		/* fall through */
	case PCAN_USBPRO_TXMSG4:
		i += 4;
		/* fall through */
	case PCAN_USBPRO_TXMSG0:
		*pc++ = va_arg(ap, int);
		*pc++ = va_arg(ap, int);
		*pc++ = va_arg(ap, int);
		*(__le32 *)pc = cpu_to_le32(va_arg(ap, u32));
		pc += 4;
		memcpy(pc, va_arg(ap, int *), i);
		pc += i;
		break;

	case PCAN_USBPRO_SETBTR:
	case PCAN_USBPRO_GETDEVID:
		*pc++ = va_arg(ap, int);
		pc += 2;
		*(__le32 *)pc = cpu_to_le32(va_arg(ap, u32));
		pc += 4;
		break;

	case PCAN_USBPRO_SETFILTR:
	case PCAN_USBPRO_SETBUSACT:
	case PCAN_USBPRO_SETSILENT:
		*pc++ = va_arg(ap, int);
		*(__le16 *)pc = cpu_to_le16(va_arg(ap, int));
		pc += 2;
		break;

	case PCAN_USBPRO_SETLED:
		*pc++ = va_arg(ap, int);
		*(__le16 *)pc = cpu_to_le16(va_arg(ap, int));
		pc += 2;
		*(__le32 *)pc = cpu_to_le32(va_arg(ap, u32));
		pc += 4;
		break;

	case PCAN_USBPRO_SETTS:
		pc++;
		*(__le16 *)pc = cpu_to_le16(va_arg(ap, int));
		pc += 2;
		break;

	default:
		pr_err("%s: %s(): unknown data type %02Xh (%d)\n",
			PCAN_USB_DRIVER_NAME, __func__, id, id);
		pc--;
		break;
	}

	len = pc - pm->rec_ptr;
	if (len > 0) {
		*pm->u.rec_cnt = cpu_to_le32(le32_to_cpu(*pm->u.rec_cnt) + 1);
		*pm->rec_ptr = id;

		pm->rec_ptr = pc;
		pm->rec_buffer_len += len;
	}

	va_end(ap);

	return len;
}