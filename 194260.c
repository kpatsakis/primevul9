static void __init check_parameters(void)
{
	if(buffer_size < PAGE_SIZE) {
		buffer_size = PAGE_SIZE;
		LOG("buffer_size too small, setting to %d\n", buffer_size);
	} else if(buffer_size > 1024*1024) {
		/* arbitrary upper limiit */
		buffer_size = 1024*1024;
		LOG("buffer_size ridiculously large, setting to %d\n",
		    buffer_size);
	} else {
		buffer_size += PAGE_SIZE-1;
		buffer_size &= ~(PAGE_SIZE-1);
	}

	if(num_buffers < 1) {
		num_buffers = 1;
		LOG("num_buffers too small, setting to %d\n", num_buffers);
	} else if(num_buffers > VIDEO_MAX_FRAME) {
		num_buffers = VIDEO_MAX_FRAME;
		LOG("num_buffers too large, setting to %d\n", num_buffers);
	}

	if(alternate < USBIF_ISO_1 || alternate > USBIF_ISO_6) {
		alternate = DEFAULT_ALT;
		LOG("alternate specified is invalid, using %d\n", alternate);
	}

	if (flicker_mode != 0 && flicker_mode != FLICKER_50 && flicker_mode != FLICKER_60) {
		flicker_mode = 0;
		LOG("Flicker mode specified is invalid, using %d\n",
		    flicker_mode);
	}

	DBG("Using %d buffers, each %d bytes, alternate=%d\n",
	    num_buffers, buffer_size, alternate);
}