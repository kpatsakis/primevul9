static bool load_buffer(RzBinFile *bf, RzBinObject *obj, RzBuffer *buf, Sdb *sdb) {
	lmf_record lrec;
	lmf_resource lres;
	lmf_data ldata;
	ut64 offset = QNX_RECORD_SIZE;

	QnxObj *qo = RZ_NEW0(QnxObj);
	if (!qo) {
		return false;
	}

	RzList *sections = rz_list_newf((RzListFree)rz_bin_section_free);
	RzList *maps = rz_list_newf((RzListFree)rz_bin_map_free);
	RzList *fixups = rz_list_newf(free);
	if (!sections || !maps || !fixups) {
		goto beach;
	}
	qo->kv = sdb_new0();
	if (!qo->kv) {
		goto beach;
	}
	// Read the first record
	if (rz_buf_fread_at(bf->buf, 0, (ut8 *)&lrec, "ccss", 1) < QNX_RECORD_SIZE) {
		goto beach;
	}
	// Load the header
	lmf_header_load(&qo->lmfh, bf->buf, qo->kv);
	offset += lrec.data_nbytes;

	for (;;) {
		if (rz_buf_fread_at(bf->buf, offset, (ut8 *)&lrec, "ccss", 1) < QNX_RECORD_SIZE) {
			goto beach;
		}
		offset += sizeof(lmf_record);

		if (lrec.rec_type == LMF_IMAGE_END_REC) {
			break;
		} else if (lrec.rec_type == LMF_RESOURCE_REC) {
			if (rz_buf_fread_at(bf->buf, offset, (ut8 *)&lres, "ssss", 1) < sizeof(lmf_resource)) {
				goto beach;
			}
			RzBinSection *ptr = RZ_NEW0(RzBinSection);
			if (!ptr) {
				goto beach;
			}
			ptr->name = strdup("LMF_RESOURCE");
			ptr->paddr = offset;
			ptr->vsize = lrec.data_nbytes - sizeof(lmf_resource);
			ptr->size = ptr->vsize;
			rz_list_append(sections, ptr);

			RzBinMap *map = RZ_NEW0(RzBinMap);
			if (!map) {
				goto beach;
			}
			map->name = ptr->name ? strdup(ptr->name) : NULL;
			map->paddr = ptr->paddr;
			map->psize = ptr->size;
			map->vsize = ptr->vsize;
			rz_list_append(maps, map);
		} else if (lrec.rec_type == LMF_LOAD_REC) {
			RzBinSection *ptr = RZ_NEW0(RzBinSection);
			if (!ptr) {
				goto beach;
			}
			if (rz_buf_fread_at(bf->buf, offset, (ut8 *)&ldata, "si", 1) < sizeof(lmf_data)) {
				free(ptr);
				goto beach;
			}
			ptr->name = strdup("LMF_LOAD");
			ptr->paddr = offset;
			ptr->vaddr = ldata.offset;
			ptr->vsize = lrec.data_nbytes - sizeof(lmf_data);
			ptr->size = ptr->vsize;
			rz_list_append(sections, ptr);

			RzBinMap *map = RZ_NEW0(RzBinMap);
			if (!map) {
				goto beach;
			}
			map->name = ptr->name ? strdup(ptr->name) : NULL;
			map->paddr = ptr->paddr;
			map->psize = ptr->size;
			map->vsize = ptr->vsize;
			rz_list_append(maps, map);
		} else if (lrec.rec_type == LMF_FIXUP_REC) {
			RzBinReloc *ptr = RZ_NEW0(RzBinReloc);
			if (!ptr || rz_buf_fread_at(bf->buf, offset, (ut8 *)&ldata, "si", 1) < sizeof(lmf_data)) {
				free(ptr);
				goto beach;
			}
			ptr->vaddr = ptr->paddr = ldata.offset;
			ptr->type = 'f'; // "LMF_FIXUP";
			rz_list_append(fixups, ptr);
		} else if (lrec.rec_type == LMF_8087_FIXUP_REC) {
			RzBinReloc *ptr = RZ_NEW0(RzBinReloc);
			if (!ptr || rz_buf_fread_at(bf->buf, offset, (ut8 *)&ldata, "si", 1) < sizeof(lmf_data)) {
				free(ptr);
				goto beach;
			}
			ptr->vaddr = ptr->paddr = ldata.offset;
			ptr->type = 'F'; // "LMF_8087_FIXUP";
			rz_list_append(fixups, ptr);
		} else if (lrec.rec_type == LMF_RW_END_REC) {
			rz_buf_fread_at(bf->buf, offset, (ut8 *)&qo->rwend, "si", 1);
		}
		offset += lrec.data_nbytes;
	}
	sdb_ns_set(sdb, "info", qo->kv);
	qo->sections = sections;
	qo->maps = maps;
	qo->fixups = fixups;
	obj->bin_obj = qo;
	return true;
beach:
	free(qo);
	rz_list_free(fixups);
	rz_list_free(maps);
	rz_list_free(sections);
	return false;
}