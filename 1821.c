GF_Err gf_odf_parse_descriptor(GF_BitStream *bs, GF_Descriptor **desc, u32 *desc_size)
{
	u32 val, size, sizeHeader;
	u8 tag;
	GF_Err err;
	GF_Descriptor *newDesc;
	if (!bs) return GF_BAD_PARAM;

	*desc_size = 0;

	//tag
	tag = (u8) gf_bs_read_int(bs, 8);
	sizeHeader = 1;

	//size
	size = 0;
	do {
		val = gf_bs_read_int(bs, 8);
		sizeHeader++;
		if (sizeHeader > 5) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CODEC, ("[ODF] Descriptor size on more than 4 bytes\n"));
			return GF_ODF_INVALID_DESCRIPTOR;
		}
		size <<= 7;
		size |= val & 0x7F;
	} while ( val & 0x80);
	*desc_size = size;

	if (gf_bs_available(bs) < size) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODEC, ("[ODF] Not enough bytes (%d) to read descriptor (size=%d)\n", gf_bs_available(bs), size));
		return GF_ODF_INVALID_DESCRIPTOR;
	}

	GF_LOG(GF_LOG_DEBUG, GF_LOG_CODEC, ("[ODF] Reading descriptor (tag %d size %d)\n", tag, size ));

	newDesc = gf_odf_create_descriptor(tag);
	if (! newDesc) {
		*desc = NULL;
		*desc_size = sizeHeader;
		if ( (tag >= GF_ODF_ISO_RES_BEGIN_TAG) &&
		        (tag <= GF_ODF_ISO_RES_END_TAG) ) {
			return GF_ODF_FORBIDDEN_DESCRIPTOR;
		}
		else if (!tag || (tag == 0xFF)) {
			return GF_ODF_INVALID_DESCRIPTOR;
		}
#ifndef GPAC_MINIMAL_ODF
		return GF_OUT_OF_MEM;
#else
		gf_bs_skip_bytes(bs, size);
		*desc_size = size + sizeHeader - gf_odf_size_field_size(*desc_size);
		return GF_OK;
#endif
	}

	newDesc->tag = tag;
	err = gf_odf_read_descriptor(bs, newDesc, *desc_size);

	/*FFMPEG fix*/
	if ((tag==GF_ODF_SLC_TAG) && (((GF_SLConfig*)newDesc)->predefined==2)) {
		if (*desc_size==3) {
			*desc_size = 1;
			err = GF_OK;
		}
	}

	//little trick to handle lazy bitstreams that encode
	//SizeOfInstance on a fix number of bytes
	//This nb of bytes is added in Read methods
	*desc_size += sizeHeader - gf_odf_size_field_size(*desc_size);
	*desc = newDesc;
	if (err) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODEC, ("[ODF] Error reading descriptor (tag %d size %d): %s\n", tag, size, gf_error_to_string(err) ));
		gf_odf_delete_descriptor(newDesc);
		*desc = NULL;
	}
	return err;
}