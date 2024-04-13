int CLASS parse_tiff_ifd (int base)
{
  unsigned entries, tag, type, len, plen=16, save;
  int ifd, use_cm=0, cfa, i, j, c, ima_len=0;
  char software[64], *cbuf, *cp;
  uchar cfa_pat[16], cfa_pc[] = { 0,1,2,3 }, tab[256];
  double cc[4][4], cm[4][3], cam_xyz[4][3], num;
  double ab[]={ 1,1,1,1 }, asn[] = { 0,0,0,0 }, xyz[] = { 1,1,1 };
  unsigned sony_curve[] = { 0,0,0,0,0,4095 };
  unsigned *buf, sony_offset=0, sony_length=0, sony_key=0;
  struct jhead jh;
#ifndef LIBRAW_LIBRARY_BUILD
  FILE *sfp;
#endif

  if (tiff_nifds >= sizeof tiff_ifd / sizeof tiff_ifd[0])
    return 1;
  ifd = tiff_nifds++;
  for (j=0; j < 4; j++)
    for (i=0; i < 4; i++)
      cc[j][i] = i == j;
  entries = get2();
  if (entries > 512) return 1;
  while (entries--) {
    tiff_get (base, &tag, &type, &len, &save);
    switch (tag) {
      case 5:   width  = get2();  break;
      case 6:   height = get2();  break;
      case 7:   width += get2();  break;
      case 9:   if ((i = get2())) filters = i;  break;
      case 17: case 18:
	if (type == 3 && len == 1)
	  cam_mul[(tag-17)*2] = get2() / 256.0;
	break;
      case 23:
	if (type == 3) iso_speed = get2();
	break;
      case 36: case 37: case 38:
	cam_mul[tag-0x24] = get2();
	break;
      case 39:
	if (len < 50 || cam_mul[0]) break;
	fseek (ifp, 12, SEEK_CUR);
	FORC3 cam_mul[c] = get2();
	break;
      case 46:
	if (type != 7 || fgetc(ifp) != 0xff || fgetc(ifp) != 0xd8) break;
	thumb_offset = ftell(ifp) - 2;
	thumb_length = len;
	break;
      case 61440:			/* Fuji HS10 table */
	parse_tiff_ifd (base);
	break;
      case 2: case 256: case 61441:	/* ImageWidth */
	tiff_ifd[ifd].t_width = getint(type);
	break;
      case 3: case 257: case 61442:	/* ImageHeight */
	tiff_ifd[ifd].t_height = getint(type);
	break;
      case 258:				/* BitsPerSample */
      case 61443:
	tiff_ifd[ifd].samples = len & 7;
	tiff_ifd[ifd].bps = getint(type);
	break;
      case 61446:
	raw_height = 0;
	if (tiff_ifd[ifd].bps > 12) break;
	load_raw = &CLASS packed_load_raw;
	load_flags = get4() ? 24:80;
	break;
      case 259:				/* Compression */
	tiff_ifd[ifd].comp = getint(type);
	break;
      case 262:				/* PhotometricInterpretation */
	tiff_ifd[ifd].phint = get2();
	break;
      case 270:				/* ImageDescription */
	fread (desc, 512, 1, ifp);
	break;
      case 271:				/* Make */
	fgets (make, 64, ifp);
	break;
      case 272:				/* Model */
	fgets (model, 64, ifp);
	break;
      case 280:				/* Panasonic RW2 offset */
	if (type != 4) break;
	load_raw = &CLASS panasonic_load_raw;
	load_flags = 0x2008;
      case 273:				/* StripOffset */
      case 513:				/* JpegIFOffset */
      case 61447:
	tiff_ifd[ifd].offset = get4()+base;
	if (!tiff_ifd[ifd].bps && tiff_ifd[ifd].offset > 0) {
	  fseek (ifp, tiff_ifd[ifd].offset, SEEK_SET);
	  if (ljpeg_start (&jh, 1)) {
	    tiff_ifd[ifd].comp    = 6;
	    tiff_ifd[ifd].t_width   = jh.wide;
	    tiff_ifd[ifd].t_height  = jh.high;
	    tiff_ifd[ifd].bps     = jh.bits;
	    tiff_ifd[ifd].samples = jh.clrs;
	    if (!(jh.sraw || (jh.clrs & 1)))
	      tiff_ifd[ifd].t_width *= jh.clrs;
	    i = order;
	    parse_tiff (tiff_ifd[ifd].offset + 12);
	    order = i;
	  }
	}
	break;
      case 274:				/* Orientation */
	tiff_ifd[ifd].t_flip = "50132467"[get2() & 7]-'0';
	break;
      case 277:				/* SamplesPerPixel */
	tiff_ifd[ifd].samples = getint(type) & 7;
	break;
      case 279:				/* StripByteCounts */
      case 514:
      case 61448:
	tiff_ifd[ifd].bytes = get4();
	break;
      case 61454:
	FORC3 cam_mul[(4-c) % 3] = getint(type);
	break;
      case 305:  case 11:		/* Software */
	fgets (software, 64, ifp);
	if (!strncmp(software,"Adobe",5) ||
	    !strncmp(software,"dcraw",5) ||
	    !strncmp(software,"UFRaw",5) ||
	    !strncmp(software,"Bibble",6) ||
	    !strncmp(software,"Nikon Scan",10) ||
	    !strcmp (software,"Digital Photo Professional"))
	  is_raw = 0;
	break;
      case 306:				/* DateTime */
	get_timestamp(0);
	break;
      case 315:				/* Artist */
	fread (artist, 64, 1, ifp);
	break;
      case 322:				/* TileWidth */
	tiff_ifd[ifd].t_tile_width = getint(type);
	break;
      case 323:				/* TileLength */
	tiff_ifd[ifd].t_tile_length = getint(type);
	break;
      case 324:				/* TileOffsets */
	tiff_ifd[ifd].offset = len > 1 ? ftell(ifp) : get4();
	if (len == 4) {
	  load_raw = &CLASS sinar_4shot_load_raw;
	  is_raw = 5;
	}
	break;
#ifdef LIBRAW_LIBRARY_BUILD
      case 325:				/* TileByteCount */
          tiff_ifd[ifd].tile_maxbytes = 0;
          for(int jj=0;jj<len;jj++)
              {
                  int s = get4();
                  if(s > tiff_ifd[ifd].tile_maxbytes) tiff_ifd[ifd].tile_maxbytes=s;
              }
	break;
#endif
      case 330:				/* SubIFDs */
	if (!strcmp(model,"DSLR-A100") && tiff_ifd[ifd].t_width == 3872) {
	  load_raw = &CLASS sony_arw_load_raw;
	  data_offset = get4()+base;
	  ifd++;  break;
	}
        if(len > 1000) len=1000; /* 1000 SubIFDs is enough */
	while (len--) {
	  i = ftell(ifp);
	  fseek (ifp, get4()+base, SEEK_SET);
	  if (parse_tiff_ifd (base)) break;
	  fseek (ifp, i+4, SEEK_SET);
	}
	break;
      case 400:
	strcpy (make, "Sarnoff");
	maximum = 0xfff;
	break;
      case 28688:
	FORC4 sony_curve[c+1] = get2() >> 2 & 0xfff;
	for (i=0; i < 5; i++)
	  for (j = sony_curve[i]+1; j <= sony_curve[i+1]; j++)
	    curve[j] = curve[j-1] + (1 << i);
	break;
      case 29184: sony_offset = get4();  break;
      case 29185: sony_length = get4();  break;
      case 29217: sony_key    = get4();  break;
      case 29264:
	parse_minolta (ftell(ifp));
	raw_width = 0;
	break;
      case 29443:
	FORC4 cam_mul[c ^ (c < 2)] = get2();
	break;
      case 29459:
	FORC4 cam_mul[c] = get2();
	i = (cam_mul[1] == 1024 && cam_mul[2] == 1024) << 1;
	SWAP (cam_mul[i],cam_mul[i+1])
	break;
    case 30720: // Sony matrix, Sony_SR2SubIFD_0x7800
      for (i=0; i < 3; i++)
        FORC3 cmatrix[i][c] = ((short) get2()) / 1024.0;
#ifdef DCRAW_VERBOSE
	if (verbose) fprintf (stderr, _(" Sony matrix:\n%f %f %f\n%f %f %f\n%f %f %f\n"), cmatrix[0][0],  cmatrix[0][1], cmatrix[0][2], cmatrix[1][0], cmatrix[1][1], cmatrix[1][2], cmatrix[2][0], cmatrix[2][1], cmatrix[2][2]);
#endif
	break;
    case 29456: // Sony black level, Sony_SR2SubIFD_0x7310, needs to be divided by 4
      FORC4 cblack[c ^ c >> 1] = get2()/4;
      i = cblack[3];
      FORC3 if(i>cblack[c]) i = cblack[c];
      FORC4 cblack[c]-=i;
      black = i;
#ifdef DCRAW_VERBOSE
      if (verbose) fprintf (stderr, _("...Sony black: %u cblack: %u %u %u %u\n"),black, cblack[0],cblack[1],cblack[2], cblack[3]);
#endif
      break;
      case 33405:			/* Model2 */
	fgets (model2, 64, ifp);
	break;
      case 33422:			/* CFAPattern */
      case 64777:			/* Kodak P-series */
	if ((plen=len) > 16) plen = 16;
	fread (cfa_pat, 1, plen, ifp);
	for (colors=cfa=i=0; i < plen && colors < 4; i++) {
	  colors += !(cfa & (1 << cfa_pat[i]));
	  cfa |= 1 << cfa_pat[i];
	}
	if (cfa == 070) memcpy (cfa_pc,"\003\004\005",3);	/* CMY */
	if (cfa == 072) memcpy (cfa_pc,"\005\003\004\001",4);	/* GMCY */
	goto guess_cfa_pc;
      case 33424:
      case 65024:
	fseek (ifp, get4()+base, SEEK_SET);
	parse_kodak_ifd (base);
	break;
      case 33434:			/* ExposureTime */
	shutter = getreal(type);
	break;
      case 33437:			/* FNumber */
	aperture = getreal(type);
	break;
      case 34306:			/* Leaf white balance */
	FORC4 cam_mul[c ^ 1] = 4096.0 / get2();
	break;
      case 34307:			/* Leaf CatchLight color matrix */
	fread (software, 1, 7, ifp);
	if (strncmp(software,"MATRIX",6)) break;
	colors = 4;
	for (raw_color = i=0; i < 3; i++) {
	  FORC4 fscanf (ifp, "%f", &rgb_cam[i][c^1]);
	  if (!use_camera_wb) continue;
	  num = 0;
	  FORC4 num += rgb_cam[i][c];
	  FORC4 rgb_cam[i][c] /= num;
	}
	break;
      case 34310:			/* Leaf metadata */
	parse_mos (ftell(ifp));
      case 34303:
	strcpy (make, "Leaf");
	break;
      case 34665:			/* EXIF tag */
	fseek (ifp, get4()+base, SEEK_SET);
	parse_exif (base);
	break;
      case 34853:			/* GPSInfo tag */
	fseek (ifp, get4()+base, SEEK_SET);
	parse_gps (base);
	break;
      case 34675:			/* InterColorProfile */
      case 50831:			/* AsShotICCProfile */
	profile_offset = ftell(ifp);
	profile_length = len;
	break;
      case 37122:			/* CompressedBitsPerPixel */
	kodak_cbpp = get4();
	break;
      case 37386:			/* FocalLength */
	focal_len = getreal(type);
	break;
      case 37393:			/* ImageNumber */
	shot_order = getint(type);
	break;
      case 37400:			/* old Kodak KDC tag */
	for (raw_color = i=0; i < 3; i++) {
	  getreal(type);
	  FORC3 rgb_cam[i][c] = getreal(type);
	}
	break;
      case 40976:
	strip_offset = get4();
	load_raw = &CLASS samsung_load_raw;
	break;
      case 46275:			/* Imacon tags */
	strcpy (make, "Imacon");
	data_offset = ftell(ifp);
	ima_len = len;
	break;
      case 46279:
	if (!ima_len) break;
	fseek (ifp, 38, SEEK_CUR);
      case 46274:
	fseek (ifp, 40, SEEK_CUR);
	raw_width  = get4();
	raw_height = get4();
	left_margin = get4() & 7;
	width = raw_width - left_margin - (get4() & 7);
	top_margin = get4() & 7;
	height = raw_height - top_margin - (get4() & 7);
	if (raw_width == 7262 && ima_len == 234317952 ) {
	  height = 5412;
	  width  = 7216;
	  left_margin = 7;
          filters=0;
	} else 	if (raw_width == 7262) {
	  height = 5444;
	  width  = 7244;
	  left_margin = 7;
	}
	fseek (ifp, 52, SEEK_CUR);
	FORC3 cam_mul[c] = getreal(11);
	fseek (ifp, 114, SEEK_CUR);
	flip = (get2() >> 7) * 90;
	if (width * height * 6 == ima_len) {
	  if (flip % 180 == 90) SWAP(width,height);
	  raw_width = width;
	  raw_height = height;
	  left_margin = top_margin = filters = flip = 0;
	}
	sprintf (model, "Ixpress %d-Mp", height*width/1000000);
	load_raw = &CLASS imacon_full_load_raw;
	if (filters) {
	  if (left_margin & 1) filters = 0x61616161;
	  load_raw = &CLASS unpacked_load_raw;
	}
	maximum = 0xffff;
	break;
      case 50454:			/* Sinar tag */
      case 50455:
	if (!(cbuf = (char *) malloc(len))) break;
	fread (cbuf, 1, len, ifp);
	for (cp = cbuf-1; cp && cp < cbuf+len; cp = strchr(cp,'\n'))
	  if (!strncmp (++cp,"Neutral ",8))
	    sscanf (cp+8, "%f %f %f", cam_mul, cam_mul+1, cam_mul+2);
	free (cbuf);
	break;
      case 50458:
	if (!make[0]) strcpy (make, "Hasselblad");
	break;
      case 50459:			/* Hasselblad tag */
	i = order;
	j = ftell(ifp);
	c = tiff_nifds;
	order = get2();
	fseek (ifp, j+(get2(),get4()), SEEK_SET);
	parse_tiff_ifd (j);
	maximum = 0xffff;
	tiff_nifds = c;
	order = i;
	break;
      case 50706:			/* DNGVersion */
	FORC4 dng_version = (dng_version << 8) + fgetc(ifp);
	if (!make[0]) strcpy (make, "DNG");
	is_raw = 1;
	break;
      case 50710:			/* CFAPlaneColor */
	if (len > 4) len = 4;
	colors = len;
	fread (cfa_pc, 1, colors, ifp);
guess_cfa_pc:
	FORCC tab[cfa_pc[c]] = c;
	cdesc[c] = 0;
	for (i=16; i--; )
	  filters = filters << 2 | tab[cfa_pat[i % plen]];
	filters -= !filters;
	break;
      case 50711:			/* CFALayout */
	if (get2() == 2) {
	  fuji_width = 1;
	  filters = 0x49494949;
	}
	break;
      case 291:
      case 50712:			/* LinearizationTable */
	linear_table (len);
	break;
      case 50713:			/* BlackLevelRepeatDim */
	cblack[4] = get2();
	cblack[5] = get2();
	if (cblack[4] * cblack[5] > (sizeof(cblack) / sizeof (cblack[0]) - 6))
	    cblack[4] = cblack[5] = 1;
	break;
      case 61450:
	cblack[4] = cblack[5] = MIN(sqrt(len),64);
      case 50714:			/* BlackLevel */
	FORC (cblack[4] * cblack[5])
	  cblack[6+c] = getreal(type);
	black = 0;
	break;
      case 50715:			/* BlackLevelDeltaH */
      case 50716:			/* BlackLevelDeltaV */
	for (num=i=0; i < len && i < 65536; i++)
	  num += getreal(type);
	black += num/len + 0.5;
	break;
      case 50717:			/* WhiteLevel */
	maximum = getint(type);
	break;
      case 50718:			/* DefaultScale */
	pixel_aspect  = getreal(type);
	pixel_aspect /= getreal(type);
	break;
      case 50721:			/* ColorMatrix1 */
      case 50722:			/* ColorMatrix2 */
	FORCC for (j=0; j < 3; j++)
	  cm[c][j] = getreal(type);
	use_cm = 1;
	break;
      case 50723:			/* CameraCalibration1 */
      case 50724:			/* CameraCalibration2 */
	for (i=0; i < colors; i++)
	  FORCC cc[i][c] = getreal(type);
	break;
      case 50727:			/* AnalogBalance */
	FORCC ab[c] = getreal(type);
	break;
      case 50728:			/* AsShotNeutral */
	FORCC asn[c] = getreal(type);
	break;
      case 50729:			/* AsShotWhiteXY */
	xyz[0] = getreal(type);
	xyz[1] = getreal(type);
	xyz[2] = 1 - xyz[0] - xyz[1];
	FORC3 xyz[c] /= d65_white[c];
	break;
      case 50740:			/* DNGPrivateData */
	if (dng_version) break;
	parse_minolta (j = get4()+base);
	fseek (ifp, j, SEEK_SET);
	parse_tiff_ifd (base);
	break;
      case 50752:
	read_shorts (cr2_slice, 3);
	break;
      case 50829:			/* ActiveArea */
	top_margin = getint(type);
	left_margin = getint(type);
	height = getint(type) - top_margin;
	width = getint(type) - left_margin;
	break;
      case 50830:			/* MaskedAreas */
        for (i=0; i < len && i < 32; i++)
	  mask[0][i] = getint(type);
	black = 0;
	break;
      case 51009:			/* OpcodeList2 */
	meta_offset = ftell(ifp);
	break;
      case 64772:			/* Kodak P-series */
	if (len < 13) break;
	fseek (ifp, 16, SEEK_CUR);
	data_offset = get4();
	fseek (ifp, 28, SEEK_CUR);
	data_offset += get4();
	load_raw = &CLASS packed_load_raw;
	break;
      case 65026:
	if (type == 2) fgets (model2, 64, ifp);
    }
    fseek (ifp, save, SEEK_SET);
  }
  if (sony_length && (buf = (unsigned *) malloc(sony_length))) {
    fseek (ifp, sony_offset, SEEK_SET);
    fread (buf, sony_length, 1, ifp);
    sony_decrypt (buf, sony_length/4, 1, sony_key);
#ifndef LIBRAW_LIBRARY_BUILD
    sfp = ifp;
    if ((ifp = tmpfile())) {
      fwrite (buf, sony_length, 1, ifp);
      fseek (ifp, 0, SEEK_SET);
      parse_tiff_ifd (-sony_offset);
      fclose (ifp);
    }
    ifp = sfp;
#else
    if( !ifp->tempbuffer_open(buf,sony_length))
        {
            parse_tiff_ifd(-sony_offset);
            ifp->tempbuffer_close();
        }
#endif
    free (buf);
  }
  for (i=0; i < colors; i++)
    FORCC cc[i][c] *= ab[i];
  if (use_cm) {
    FORCC for (i=0; i < 3; i++)
      for (cam_xyz[c][i]=j=0; j < colors; j++)
	cam_xyz[c][i] += cc[c][j] * cm[j][i] * xyz[i];
    cam_xyz_coeff (cmatrix, cam_xyz);
  }
  if (asn[0]) {
    cam_mul[3] = 0;
    FORCC cam_mul[c] = 1 / asn[c];
  }
  if (!use_cm)
    FORCC pre_mul[c] /= cc[c][c];
  return 0;
}