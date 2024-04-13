int vbin_printf(u32 *bin_buf, size_t size, const char *fmt, va_list args)
{
	struct printf_spec spec = {0};
	char *str, *end;
	int width;

	str = (char *)bin_buf;
	end = (char *)(bin_buf + size);

#define save_arg(type)							\
({									\
	unsigned long long value;					\
	if (sizeof(type) == 8) {					\
		unsigned long long val8;				\
		str = PTR_ALIGN(str, sizeof(u32));			\
		val8 = va_arg(args, unsigned long long);		\
		if (str + sizeof(type) <= end) {			\
			*(u32 *)str = *(u32 *)&val8;			\
			*(u32 *)(str + 4) = *((u32 *)&val8 + 1);	\
		}							\
		value = val8;						\
	} else {							\
		unsigned int val4;					\
		str = PTR_ALIGN(str, sizeof(type));			\
		val4 = va_arg(args, int);				\
		if (str + sizeof(type) <= end)				\
			*(typeof(type) *)str = (type)(long)val4;	\
		value = (unsigned long long)val4;			\
	}								\
	str += sizeof(type);						\
	value;								\
})

	while (*fmt) {
		int read = format_decode(fmt, &spec);

		fmt += read;

		switch (spec.type) {
		case FORMAT_TYPE_NONE:
		case FORMAT_TYPE_PERCENT_CHAR:
			break;
		case FORMAT_TYPE_INVALID:
			goto out;

		case FORMAT_TYPE_WIDTH:
		case FORMAT_TYPE_PRECISION:
			width = (int)save_arg(int);
			/* Pointers may require the width */
			if (*fmt == 'p')
				set_field_width(&spec, width);
			break;

		case FORMAT_TYPE_CHAR:
			save_arg(char);
			break;

		case FORMAT_TYPE_STR: {
			const char *save_str = va_arg(args, char *);
			size_t len;

			if ((unsigned long)save_str > (unsigned long)-PAGE_SIZE
					|| (unsigned long)save_str < PAGE_SIZE)
				save_str = "(null)";
			len = strlen(save_str) + 1;
			if (str + len < end)
				memcpy(str, save_str, len);
			str += len;
			break;
		}

		case FORMAT_TYPE_PTR:
			/* Dereferenced pointers must be done now */
			switch (*fmt) {
			/* Dereference of functions is still OK */
			case 'S':
			case 's':
			case 'F':
			case 'f':
				save_arg(void *);
				break;
			default:
				if (!isalnum(*fmt)) {
					save_arg(void *);
					break;
				}
				str = pointer(fmt, str, end, va_arg(args, void *),
					      spec);
				if (str + 1 < end)
					*str++ = '\0';
				else
					end[-1] = '\0'; /* Must be nul terminated */
			}
			/* skip all alphanumeric pointer suffixes */
			while (isalnum(*fmt))
				fmt++;
			break;

		default:
			switch (spec.type) {

			case FORMAT_TYPE_LONG_LONG:
				save_arg(long long);
				break;
			case FORMAT_TYPE_ULONG:
			case FORMAT_TYPE_LONG:
				save_arg(unsigned long);
				break;
			case FORMAT_TYPE_SIZE_T:
				save_arg(size_t);
				break;
			case FORMAT_TYPE_PTRDIFF:
				save_arg(ptrdiff_t);
				break;
			case FORMAT_TYPE_UBYTE:
			case FORMAT_TYPE_BYTE:
				save_arg(char);
				break;
			case FORMAT_TYPE_USHORT:
			case FORMAT_TYPE_SHORT:
				save_arg(short);
				break;
			default:
				save_arg(int);
			}
		}
	}

out:
	return (u32 *)(PTR_ALIGN(str, sizeof(u32))) - bin_buf;
#undef save_arg
}