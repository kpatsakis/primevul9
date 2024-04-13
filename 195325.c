void logfc(struct fc_log *log, const char *prefix, char level, const char *fmt, ...)
{
	va_list va;
	struct va_format vaf = {.fmt = fmt, .va = &va};

	va_start(va, fmt);
	if (!log) {
		switch (level) {
		case 'w':
			printk(KERN_WARNING "%s%s%pV\n", prefix ? prefix : "",
						prefix ? ": " : "", &vaf);
			break;
		case 'e':
			printk(KERN_ERR "%s%s%pV\n", prefix ? prefix : "",
						prefix ? ": " : "", &vaf);
			break;
		default:
			printk(KERN_NOTICE "%s%s%pV\n", prefix ? prefix : "",
						prefix ? ": " : "", &vaf);
			break;
		}
	} else {
		unsigned int logsize = ARRAY_SIZE(log->buffer);
		u8 index;
		char *q = kasprintf(GFP_KERNEL, "%c %s%s%pV\n", level,
						prefix ? prefix : "",
						prefix ? ": " : "", &vaf);

		index = log->head & (logsize - 1);
		BUILD_BUG_ON(sizeof(log->head) != sizeof(u8) ||
			     sizeof(log->tail) != sizeof(u8));
		if ((u8)(log->head - log->tail) == logsize) {
			/* The buffer is full, discard the oldest message */
			if (log->need_free & (1 << index))
				kfree(log->buffer[index]);
			log->tail++;
		}

		log->buffer[index] = q ? q : "OOM: Can't store error string";
		if (q)
			log->need_free |= 1 << index;
		else
			log->need_free &= ~(1 << index);
		log->head++;
	}
	va_end(va);
}