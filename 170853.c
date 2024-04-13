int bstr_printf(char *buf, size_t size, const char *fmt, const u32 *bin_buf)
{
	struct printf_spec spec = {0};
	char *str, *end;
	const char *args = (const char *)bin_buf;

	if (WARN_ON_ONCE(size > INT_MAX))
		return 0;

	str = buf;
	end = buf + size;

#define get_arg(type)							\
({									\
	typeof(type) value;						\
	if (sizeof(type) == 8) {					\
		args = PTR_ALIGN(args, sizeof(u32));			\
		*(u32 *)&value = *(u32 *)args;				\
		*((u32 *)&value + 1) = *(u32 *)(args + 4);		\
	} else {							\
		args = PTR_ALIGN(args, sizeof(type));			\
		value = *(typeof(type) *)args;				\
	}								\
	args += sizeof(type);						\
	value;								\
})

	/* Make sure end is always >= buf */
	if (end < buf) {
		end = ((void *)-1);
		size = end - buf;
	}

	while (*fmt) {
		const char *old_fmt = fmt;
		int read = format_decode(fmt, &spec);

		fmt += read;

		switch (spec.type) {
		case FORMAT_TYPE_NONE: {
			int copy = read;
			if (str < end) {
				if (copy > end - str)
					copy = end - str;
				memcpy(str, old_fmt, copy);
			}
			str += read;
			break;
		}

		case FORMAT_TYPE_WIDTH:
			set_field_width(&spec, get_arg(int));
			break;

		case FORMAT_TYPE_PRECISION:
			set_precision(&spec, get_arg(int));
			break;

		case FORMAT_TYPE_CHAR: {
			char c;

			if (!(spec.flags & LEFT)) {
				while (--spec.field_width > 0) {
					if (str < end)
						*str = ' ';
					++str;
				}
			}
			c = (unsigned char) get_arg(char);
			if (str < end)
				*str = c;
			++str;
			while (--spec.field_width > 0) {
				if (str < end)
					*str = ' ';
				++str;
			}
			break;
		}

		case FORMAT_TYPE_STR: {
			const char *str_arg = args;
			args += strlen(str_arg) + 1;
			str = string(str, end, (char *)str_arg, spec);
			break;
		}

		case FORMAT_TYPE_PTR: {
			bool process = false;
			int copy, len;
			/* Non function dereferences were already done */
			switch (*fmt) {
			case 'S':
			case 's':
			case 'F':
			case 'f':
				process = true;
				break;
			default:
				if (!isalnum(*fmt)) {
					process = true;
					break;
				}
				/* Pointer dereference was already processed */
				if (str < end) {
					len = copy = strlen(args);
					if (copy > end - str)
						copy = end - str;
					memcpy(str, args, copy);
					str += len;
					args += len;
				}
			}
			if (process)
				str = pointer(fmt, str, end, get_arg(void *), spec);

			while (isalnum(*fmt))
				fmt++;
			break;
		}

		case FORMAT_TYPE_PERCENT_CHAR:
			if (str < end)
				*str = '%';
			++str;
			break;

		case FORMAT_TYPE_INVALID:
			goto out;

		default: {
			unsigned long long num;

			switch (spec.type) {

			case FORMAT_TYPE_LONG_LONG:
				num = get_arg(long long);
				break;
			case FORMAT_TYPE_ULONG:
			case FORMAT_TYPE_LONG:
				num = get_arg(unsigned long);
				break;
			case FORMAT_TYPE_SIZE_T:
				num = get_arg(size_t);
				break;
			case FORMAT_TYPE_PTRDIFF:
				num = get_arg(ptrdiff_t);
				break;
			case FORMAT_TYPE_UBYTE:
				num = get_arg(unsigned char);
				break;
			case FORMAT_TYPE_BYTE:
				num = get_arg(signed char);
				break;
			case FORMAT_TYPE_USHORT:
				num = get_arg(unsigned short);
				break;
			case FORMAT_TYPE_SHORT:
				num = get_arg(short);
				break;
			case FORMAT_TYPE_UINT:
				num = get_arg(unsigned int);
				break;
			default:
				num = get_arg(int);
			}

			str = number(str, end, num, spec);
		} /* default: */
		} /* switch(spec.type) */
	} /* while(*fmt) */

out:
	if (size > 0) {
		if (str < end)
			*str = '\0';
		else
			end[-1] = '\0';
	}

#undef get_arg

	/* the trailing null byte doesn't count towards the total */
	return str - buf;
}