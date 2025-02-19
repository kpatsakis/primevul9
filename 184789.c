static int exif_process_IFD_in_TIFF(image_info_type *ImageInfo, size_t dir_offset, int section_index TSRMLS_DC)
{
	int i, sn, num_entries, sub_section_index = 0;
	unsigned char *dir_entry;
	char tagname[64];
	size_t ifd_size, dir_size, entry_offset, next_offset, entry_length, entry_value=0, fgot;
	int entry_tag , entry_type;
	tag_table_type tag_table = exif_get_tag_table(section_index);

	if (ImageInfo->ifd_nesting_level > MAX_IFD_NESTING_LEVEL) {
                return FALSE;
        }

	if (ImageInfo->FileSize >= dir_offset+2) {
		sn = exif_file_sections_add(ImageInfo, M_PSEUDO, 2, NULL);
#ifdef EXIF_DEBUG
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF: filesize(x%04X), IFD dir(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, 2);
#endif
		php_stream_seek(ImageInfo->infile, dir_offset, SEEK_SET); /* we do not know the order of sections */
		php_stream_read(ImageInfo->infile, (char*)ImageInfo->file.list[sn].data, 2);
		num_entries = php_ifd_get16u(ImageInfo->file.list[sn].data, ImageInfo->motorola_intel);
		dir_size = 2/*num dir entries*/ +12/*length of entry*/*num_entries +4/* offset to next ifd (points to thumbnail or NULL)*/;
		if (ImageInfo->FileSize >= dir_offset+dir_size) {
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF: filesize(x%04X), IFD dir(x%04X + x%04X), IFD entries(%d)", ImageInfo->FileSize, dir_offset+2, dir_size-2, num_entries);
#endif
			if (exif_file_sections_realloc(ImageInfo, sn, dir_size TSRMLS_CC)) {
				return FALSE;
			}
			php_stream_read(ImageInfo->infile, (char*)(ImageInfo->file.list[sn].data+2), dir_size-2);
			/*exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Dump: %s", exif_char_dump(ImageInfo->file.list[sn].data, dir_size, 0));*/
			next_offset = php_ifd_get32u(ImageInfo->file.list[sn].data + dir_size - 4, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF done, next offset x%04X", next_offset);
#endif
			/* now we have the directory we can look how long it should be */
			ifd_size = dir_size;
			for(i=0;i<num_entries;i++) {
				dir_entry 	 = ImageInfo->file.list[sn].data+2+i*12;
				entry_tag    = php_ifd_get16u(dir_entry+0, ImageInfo->motorola_intel);
				entry_type   = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
				if (entry_type > NUM_FORMATS) {
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF: tag(0x%04X,%12s): Illegal format code 0x%04X, switching to BYTE", entry_tag, exif_get_tagname(entry_tag, tagname, -12, tag_table TSRMLS_CC), entry_type);
					/* Since this is repeated in exif_process_IFD_TAG make it a notice here */
					/* and make it a warning in the exif_process_IFD_TAG which is called    */
					/* elsewhere. */
					entry_type = TAG_FMT_BYTE;
					/*The next line would break the image on writeback: */
					/* php_ifd_set16u(dir_entry+2, entry_type, ImageInfo->motorola_intel);*/
				}
				entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel) * php_tiff_bytes_per_format[entry_type];
				if (entry_length <= 4) {
					switch(entry_type) {
						case TAG_FMT_USHORT:
							entry_value  = php_ifd_get16u(dir_entry+8, ImageInfo->motorola_intel);
							break;
						case TAG_FMT_SSHORT:
							entry_value  = php_ifd_get16s(dir_entry+8, ImageInfo->motorola_intel);
							break;
						case TAG_FMT_ULONG:
							entry_value  = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
							break;
						case TAG_FMT_SLONG:
							entry_value  = php_ifd_get32s(dir_entry+8, ImageInfo->motorola_intel);
							break;
					}
					switch(entry_tag) {
						case TAG_IMAGEWIDTH:
						case TAG_COMP_IMAGE_WIDTH:
							ImageInfo->Width  = entry_value;
							break;
						case TAG_IMAGEHEIGHT:
						case TAG_COMP_IMAGE_HEIGHT:
							ImageInfo->Height = entry_value;
							break;
						case TAG_PHOTOMETRIC_INTERPRETATION:
							switch (entry_value) {
								case PMI_BLACK_IS_ZERO:
								case PMI_WHITE_IS_ZERO:
								case PMI_TRANSPARENCY_MASK:
									ImageInfo->IsColor = 0;
									break;
								case PMI_RGB:
								case PMI_PALETTE_COLOR:
								case PMI_SEPARATED:
								case PMI_YCBCR:
								case PMI_CIELAB:
									ImageInfo->IsColor = 1;
									break;
							}
							break;
					}
				} else {
					entry_offset = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
					/* if entry needs expading ifd cache and entry is at end of current ifd cache. */
					/* otherwise there may be huge holes between two entries */
					if (entry_offset + entry_length > dir_offset + ifd_size
					  && entry_offset == dir_offset + ifd_size) {
						ifd_size = entry_offset + entry_length - dir_offset;
#ifdef EXIF_DEBUG
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Resize struct: x%04X + x%04X - x%04X = x%04X", entry_offset, entry_length, dir_offset, ifd_size);
#endif
					}
				}
			}
			if (ImageInfo->FileSize >= dir_offset + ImageInfo->file.list[sn].size) {
				if (ifd_size > dir_size) {
					if (dir_offset + ifd_size > ImageInfo->FileSize) {
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Error in TIFF: filesize(x%04X) less than size of IFD(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, ifd_size);
						return FALSE;
					}
					if (exif_file_sections_realloc(ImageInfo, sn, ifd_size TSRMLS_CC)) {
						return FALSE;
					}
					/* read values not stored in directory itself */
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF: filesize(x%04X), IFD(x%04X + x%04X)", ImageInfo->FileSize, dir_offset, ifd_size);
#endif
					php_stream_read(ImageInfo->infile, (char*)(ImageInfo->file.list[sn].data+dir_size), ifd_size-dir_size);
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read from TIFF, done");
#endif
				}
				/* now process the tags */
				for(i=0;i<num_entries;i++) {
					dir_entry 	 = ImageInfo->file.list[sn].data+2+i*12;
					entry_tag    = php_ifd_get16u(dir_entry+0, ImageInfo->motorola_intel);
					entry_type   = php_ifd_get16u(dir_entry+2, ImageInfo->motorola_intel);
					/*entry_length = php_ifd_get32u(dir_entry+4, ImageInfo->motorola_intel);*/
					if (entry_tag == TAG_EXIF_IFD_POINTER ||
						entry_tag == TAG_INTEROP_IFD_POINTER ||
						entry_tag == TAG_GPS_IFD_POINTER ||
						entry_tag == TAG_SUB_IFD
					) {
						switch(entry_tag) {
							case TAG_EXIF_IFD_POINTER:
								ImageInfo->sections_found |= FOUND_EXIF;
								sub_section_index = SECTION_EXIF;
								break;
							case TAG_GPS_IFD_POINTER:
								ImageInfo->sections_found |= FOUND_GPS;
								sub_section_index = SECTION_GPS;
								break;
							case TAG_INTEROP_IFD_POINTER:
								ImageInfo->sections_found |= FOUND_INTEROP;
								sub_section_index = SECTION_INTEROP;
								break;
							case TAG_SUB_IFD:
								ImageInfo->sections_found |= FOUND_THUMBNAIL;
								sub_section_index = SECTION_THUMBNAIL;
								break;
						}
						entry_offset = php_ifd_get32u(dir_entry+8, ImageInfo->motorola_intel);
#ifdef EXIF_DEBUG
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Next IFD: %s @x%04X", exif_get_sectionname(sub_section_index), entry_offset);
#endif
						ImageInfo->ifd_nesting_level++;
						exif_process_IFD_in_TIFF(ImageInfo, entry_offset, sub_section_index TSRMLS_CC);
						if (section_index!=SECTION_THUMBNAIL && entry_tag==TAG_SUB_IFD) {
							if (ImageInfo->Thumbnail.filetype != IMAGE_FILETYPE_UNKNOWN
							&&  ImageInfo->Thumbnail.size
							&&  ImageInfo->Thumbnail.offset
							&&  ImageInfo->read_thumbnail
							) {
#ifdef EXIF_DEBUG
								exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "%s THUMBNAIL @0x%04X + 0x%04X", ImageInfo->Thumbnail.data ? "Ignore" : "Read", ImageInfo->Thumbnail.offset, ImageInfo->Thumbnail.size);
#endif
								if (!ImageInfo->Thumbnail.data) {
									ImageInfo->Thumbnail.data = safe_emalloc(ImageInfo->Thumbnail.size, 1, 0);
									php_stream_seek(ImageInfo->infile, ImageInfo->Thumbnail.offset, SEEK_SET);
									fgot = php_stream_read(ImageInfo->infile, ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size);
									if (fgot < ImageInfo->Thumbnail.size) {
										EXIF_ERRLOG_THUMBEOF(ImageInfo)
										efree(ImageInfo->Thumbnail.data);
										ImageInfo->Thumbnail.data = NULL;
									} else {
										exif_thumbnail_build(ImageInfo TSRMLS_CC);
									}
								}
							}
						}
#ifdef EXIF_DEBUG
						exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Next IFD: %s done", exif_get_sectionname(sub_section_index));
#endif
					} else {
						if (!exif_process_IFD_TAG(ImageInfo, (char*)dir_entry,
												  (char*)(ImageInfo->file.list[sn].data-dir_offset),
												  ifd_size, 0, section_index, 0, tag_table TSRMLS_CC)) {
							return FALSE;
						}
					}
				}
				/* If we had a thumbnail in a SUB_IFD we have ANOTHER image in NEXT IFD */
				if (next_offset && section_index != SECTION_THUMBNAIL) {
					/* this should be a thumbnail IFD */
					/* the thumbnail itself is stored at Tag=StripOffsets */
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read next IFD (THUMBNAIL) at x%04X", next_offset);
#endif
					ImageInfo->ifd_nesting_level++;
					exif_process_IFD_in_TIFF(ImageInfo, next_offset, SECTION_THUMBNAIL TSRMLS_CC);
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "%s THUMBNAIL @0x%04X + 0x%04X", ImageInfo->Thumbnail.data ? "Ignore" : "Read", ImageInfo->Thumbnail.offset, ImageInfo->Thumbnail.size);
#endif
					if (!ImageInfo->Thumbnail.data && ImageInfo->Thumbnail.offset && ImageInfo->Thumbnail.size && ImageInfo->read_thumbnail) {
						ImageInfo->Thumbnail.data = safe_emalloc(ImageInfo->Thumbnail.size, 1, 0);
						php_stream_seek(ImageInfo->infile, ImageInfo->Thumbnail.offset, SEEK_SET);
						fgot = php_stream_read(ImageInfo->infile, ImageInfo->Thumbnail.data, ImageInfo->Thumbnail.size);
						if (fgot < ImageInfo->Thumbnail.size) {
							EXIF_ERRLOG_THUMBEOF(ImageInfo)
							efree(ImageInfo->Thumbnail.data);
							ImageInfo->Thumbnail.data = NULL;
						} else {
							exif_thumbnail_build(ImageInfo TSRMLS_CC);
						}
					}
#ifdef EXIF_DEBUG
					exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_NOTICE, "Read next IFD (THUMBNAIL) done");
#endif
				}
				return TRUE;
			} else {
				exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Error in TIFF: filesize(x%04X) less than size of IFD(x%04X)", ImageInfo->FileSize, dir_offset+ImageInfo->file.list[sn].size);
				return FALSE;
			}
		} else {
			exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Error in TIFF: filesize(x%04X) less than size of IFD dir(x%04X)", ImageInfo->FileSize, dir_offset+dir_size);
			return FALSE;
		}
	} else {
		exif_error_docref(NULL EXIFERR_CC, ImageInfo, E_WARNING, "Error in TIFF: filesize(x%04X) less than start of IFD dir(x%04X)", ImageInfo->FileSize, dir_offset+2);
		return FALSE;
	}
}