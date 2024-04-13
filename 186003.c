static int serial_struct_ioctl(unsigned fd, unsigned cmd,
			struct serial_struct32 __user *ss32)
{
        typedef struct serial_struct32 SS32;
        int err;
        struct serial_struct ss;
        mm_segment_t oldseg = get_fs();
        __u32 udata;
	unsigned int base;

        if (cmd == TIOCSSERIAL) {
                if (!access_ok(VERIFY_READ, ss32, sizeof(SS32)))
                        return -EFAULT;
                if (__copy_from_user(&ss, ss32, offsetof(SS32, iomem_base)))
			return -EFAULT;
                if (__get_user(udata, &ss32->iomem_base))
			return -EFAULT;
                ss.iomem_base = compat_ptr(udata);
                if (__get_user(ss.iomem_reg_shift, &ss32->iomem_reg_shift) ||
		    __get_user(ss.port_high, &ss32->port_high))
			return -EFAULT;
                ss.iomap_base = 0UL;
        }
        set_fs(KERNEL_DS);
                err = sys_ioctl(fd,cmd,(unsigned long)(&ss));
        set_fs(oldseg);
        if (cmd == TIOCGSERIAL && err >= 0) {
                if (!access_ok(VERIFY_WRITE, ss32, sizeof(SS32)))
                        return -EFAULT;
                if (__copy_to_user(ss32,&ss,offsetof(SS32,iomem_base)))
			return -EFAULT;
		base = (unsigned long)ss.iomem_base  >> 32 ?
			0xffffffff : (unsigned)(unsigned long)ss.iomem_base;
		if (__put_user(base, &ss32->iomem_base) ||
		    __put_user(ss.iomem_reg_shift, &ss32->iomem_reg_shift) ||
		    __put_user(ss.port_high, &ss32->port_high))
			return -EFAULT;
        }
        return err;
}