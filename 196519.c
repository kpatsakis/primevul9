void CLASS identify()
{
  static const short pana[][6] = {
    { 3130, 1743,  4,  0, -6,  0 },
    { 3130, 2055,  4,  0, -6,  0 },
    { 3130, 2319,  4,  0, -6,  0 },
    { 3170, 2103, 18,  0,-42, 20 },
    { 3170, 2367, 18, 13,-42,-21 },
    { 3177, 2367,  0,  0, -1,  0 },
    { 3304, 2458,  0,  0, -1,  0 },
    { 3330, 2463,  9,  0, -5,  0 },
    { 3330, 2479,  9,  0,-17,  4 },
    { 3370, 1899, 15,  0,-44, 20 },
    { 3370, 2235, 15,  0,-44, 20 },
    { 3370, 2511, 15, 10,-44,-21 },
    { 3690, 2751,  3,  0, -8, -3 },
    { 3710, 2751,  0,  0, -3,  0 },
    { 3724, 2450,  0,  0,  0, -2 },
    { 3770, 2487, 17,  0,-44, 19 },
    { 3770, 2799, 17, 15,-44,-19 },
    { 3880, 2170,  6,  0, -6,  0 },
    { 4060, 3018,  0,  0,  0, -2 },
    { 4290, 2391,  3,  0, -8, -1 },
    { 4330, 2439, 17, 15,-44,-19 },
    { 4508, 2962,  0,  0, -3, -4 },
    { 4508, 3330,  0,  0, -3, -6 },
  };
  static const ushort canon[][11] = {
    { 1944, 1416,   0,  0, 48,  0 },
    { 2144, 1560,   4,  8, 52,  2, 0, 0, 0, 25 },
    { 2224, 1456,  48,  6,  0,  2 },
    { 2376, 1728,  12,  6, 52,  2 },
    { 2672, 1968,  12,  6, 44,  2 },
    { 3152, 2068,  64, 12,  0,  0, 16 },
    { 3160, 2344,  44, 12,  4,  4 },
    { 3344, 2484,   4,  6, 52,  6 },
    { 3516, 2328,  42, 14,  0,  0 },
    { 3596, 2360,  74, 12,  0,  0 },
    { 3744, 2784,  52, 12,  8, 12 },
    { 3944, 2622,  30, 18,  6,  2 },
    { 3948, 2622,  42, 18,  0,  2 },
    { 3984, 2622,  76, 20,  0,  2, 14 },
    { 4104, 3048,  48, 12, 24, 12 },
    { 4116, 2178,   4,  2,  0,  0 },
    { 4152, 2772, 192, 12,  0,  0 },
    { 4160, 3124, 104, 11,  8, 65 },
    { 4176, 3062,  96, 17,  8,  0, 0, 16, 0, 7, 0x49 },
    { 4192, 3062,  96, 17, 24,  0, 0, 16, 0, 0, 0x49 },
    { 4312, 2876,  22, 18,  0,  2 },
    { 4352, 2874,  62, 18,  0,  0 },
    { 4476, 2954,  90, 34,  0,  0 },
    { 4480, 3348,  12, 10, 36, 12, 0, 0, 0, 18, 0x49 },
    { 4496, 3366,  80, 50, 12,  0 },
    { 4832, 3204,  62, 26,  0,  0 },
    { 4832, 3228,  62, 51,  0,  0 },
    { 5108, 3349,  98, 13,  0,  0 },
    { 5120, 3318, 142, 45, 62,  0 },
    { 5280, 3528,  72, 52,  0,  0 },
    { 5344, 3516, 142, 51,  0,  0 },
    { 5344, 3584, 126,100,  0,  2 },
    { 5360, 3516, 158, 51,  0,  0 },
    { 5568, 3708,  72, 38,  0,  0 },
    { 5712, 3774,  62, 20, 10,  2 },
    { 5792, 3804, 158, 51,  0,  0 },
    { 5920, 3950, 122, 80,  2,  0 },
  };
  static const struct {
    ushort id;
    char t_model[20];
  } unique[] = {
    { 0x168, "EOS 10D" },    { 0x001, "EOS-1D" },
    { 0x175, "EOS 20D" },    { 0x174, "EOS-1D Mark II" },
    { 0x234, "EOS 30D" },    { 0x232, "EOS-1D Mark II N" },
    { 0x190, "EOS 40D" },    { 0x169, "EOS-1D Mark III" },
    { 0x261, "EOS 50D" },    { 0x281, "EOS-1D Mark IV" },
    { 0x287, "EOS 60D" },    { 0x167, "EOS-1DS" },
    { 0x170, "EOS 300D" },   { 0x188, "EOS-1Ds Mark II" },
    { 0x176, "EOS 450D" },   { 0x215, "EOS-1Ds Mark III" },
    { 0x189, "EOS 350D" },   { 0x324, "EOS-1D C" },
    { 0x236, "EOS 400D" },   { 0x269, "EOS-1D X" },
    { 0x252, "EOS 500D" },   { 0x213, "EOS 5D" },
    { 0x270, "EOS 550D" },   { 0x218, "EOS 5D Mark II" },
    { 0x286, "EOS 600D" },   { 0x285, "EOS 5D Mark III" },
    { 0x301, "EOS 650D" },   { 0x302, "EOS 6D" },
    { 0x325, "EOS 70D" },    { 0x326, "EOS 700D" },   { 0x250, "EOS 7D" },
    { 0x254, "EOS 1000D" },
    { 0x288, "EOS 1100D" },
    { 0x346, "EOS 100D" },
    { 0x331, "EOS M" },
  };
  static const struct {
    ushort id;
    char t_model[20];
  } sony_unique[] = {
    {2,"DSC-R1"},
    {256,"DSLR-A100"},
    {257,"DSLR-A900"},
    {258,"DSLR-A700"},
    {259,"DSLR-A200"},
    {260,"DSLR-A350"},
    {261,"DSLR-A300"},
    {262,"DSLR-A900"},
    {263,"DSLR-A380"},
    {264,"DSLR-A330"},
    {265,"DSLR-A230"},
    {266,"DSLR-A290"},
    {269,"DSLR-A850"},
    {270,"DSLR-A850"},
    {273,"DSLR-A550"},
    {274,"DSLR-A500"},
    {275,"DSLR-A450"},
    {278,"NEX-5"},
    {279,"NEX-3"},
    {280,"SLT-A33"},
    {281,"SLT-A55"},
    {282,"DSLR-A560"},
    {283,"DSLR-A580"},
    {284,"NEX-C3"},
    {285,"SLT-A35"},
    {286,"SLT-A65"},
    {287,"SLT-A77"},
    {288,"NEX-5N"},
    {289,"NEX-7"},
    {290,"NEX-VG20E"},
    {291,"SLT-A37"},
    {292,"SLT-A57"},
    {293,"NEX-F3"},
    {294,"SLT-A99"},
    {295,"NEX-6"},
    {296,"NEX-5R"},
    {297,"DSC-RX100"},
    {298,"DSC-RX1"},
    {299,"NEX-VG900"},
    {300,"NEX-VG30E"},
    {302,"ILCE-3000"},
    {303,"SLT-A58"},
    {305,"NEX-3N"},
    {306,"ILCE-A7"},
    {307,"NEX-5T"},
    {308,"DSC-RX100M2"},
    {310,"DSC-RX1R"},
    {311,"ILCE-A7R"},
  };
  static const struct {
    unsigned fsize;
    ushort rw, rh;
    uchar lm, tm, rm, bm, lf, cf, max, flags;
    char t_make[10], t_model[20];
    ushort offset;
  } table[] = {
    {   786432,1024, 768, 0, 0, 0, 0, 0,0x94,0,0,"AVT","F-080C" },
    {  1447680,1392,1040, 0, 0, 0, 0, 0,0x94,0,0,"AVT","F-145C" },
    {  1920000,1600,1200, 0, 0, 0, 0, 0,0x94,0,0,"AVT","F-201C" },
    {  5067304,2588,1958, 0, 0, 0, 0, 0,0x94,0,0,"AVT","F-510C" },
    {  5067316,2588,1958, 0, 0, 0, 0, 0,0x94,0,0,"AVT","F-510C",12 },
    { 10134608,2588,1958, 0, 0, 0, 0, 9,0x94,0,0,"AVT","F-510C" },
    { 10134620,2588,1958, 0, 0, 0, 0, 9,0x94,0,0,"AVT","F-510C",12 },
    { 16157136,3272,2469, 0, 0, 0, 0, 9,0x94,0,0,"AVT","F-810C" },
    { 15980544,3264,2448, 0, 0, 0, 0, 8,0x61,0,1,"AgfaPhoto","DC-833m" },
    {  9631728,2532,1902, 0, 0, 0, 0,96,0x61,0,0,"Alcatel","5035D" },
    {  2868726,1384,1036, 0, 0, 0, 0,64,0x49,0,8,"Baumer","TXG14",1078 },
    {  5298000,2400,1766,12,12,44, 2,40,0x94,0,2,"Canon","PowerShot SD300" },
    {  6553440,2664,1968, 4, 4,44, 4,40,0x94,0,2,"Canon","PowerShot A460" },
    {  6573120,2672,1968,12, 8,44, 0,40,0x94,0,2,"Canon","PowerShot A610" },
    {  6653280,2672,1992,10, 6,42, 2,40,0x94,0,2,"Canon","PowerShot A530" },
    {  7710960,2888,2136,44, 8, 4, 0,40,0x94,0,2,"Canon","PowerShot S3 IS" },
    {  9219600,3152,2340,36,12, 4, 0,40,0x94,0,2,"Canon","PowerShot A620" },
    {  9243240,3152,2346,12, 7,44,13,40,0x49,0,2,"Canon","PowerShot A470" },
    { 10341600,3336,2480, 6, 5,32, 3,40,0x94,0,2,"Canon","PowerShot A720 IS" },
    { 10383120,3344,2484,12, 6,44, 6,40,0x94,0,2,"Canon","PowerShot A630" },
    { 12945240,3736,2772,12, 6,52, 6,40,0x94,0,2,"Canon","PowerShot A640" },
    { 15636240,4104,3048,48,12,24,12,40,0x94,0,2,"Canon","PowerShot A650" },
    { 15467760,3720,2772, 6,12,30, 0,40,0x94,0,2,"Canon","PowerShot SX110 IS" },
    { 15534576,3728,2778,12, 9,44, 9,40,0x94,0,2,"Canon","PowerShot SX120 IS" },
    { 18653760,4080,3048,24,12,24,12,40,0x94,0,2,"Canon","PowerShot SX20 IS" },
    { 19131120,4168,3060,92,16, 4, 1,40,0x94,0,2,"Canon","PowerShot SX220 HS" },
    { 21936096,4464,3276,25,10,73,12,40,0x16,0,2,"Canon","PowerShot SX30 IS" },
    { 24724224,4704,3504, 8,16,56, 8,40,0x49,0,2,"Canon","PowerShot A3300 IS" },
    {  1976352,1632,1211, 0, 2, 0, 1, 0,0x94,0,1,"Casio","QV-2000UX" },
    {  3217760,2080,1547, 0, 0,10, 1, 0,0x94,0,1,"Casio","QV-3*00EX" },
    {  6218368,2585,1924, 0, 0, 9, 0, 0,0x94,0,1,"Casio","QV-5700" },
    {  7816704,2867,2181, 0, 0,34,36, 0,0x16,0,1,"Casio","EX-Z60" },
    {  2937856,1621,1208, 0, 0, 1, 0, 0,0x94,7,13,"Casio","EX-S20" },
    {  4948608,2090,1578, 0, 0,32,34, 0,0x94,7,1,"Casio","EX-S100" },
    {  6054400,2346,1720, 2, 0,32, 0, 0,0x94,7,1,"Casio","QV-R41" },
    {  7426656,2568,1928, 0, 0, 0, 0, 0,0x94,0,1,"Casio","EX-P505" },
    {  7530816,2602,1929, 0, 0,22, 0, 0,0x94,7,1,"Casio","QV-R51" },
    {  7542528,2602,1932, 0, 0,32, 0, 0,0x94,7,1,"Casio","EX-Z50" },
    {  7562048,2602,1937, 0, 0,25, 0, 0,0x16,7,1,"Casio","EX-Z500" },
    {  7753344,2602,1986, 0, 0,32,26, 0,0x94,7,1,"Casio","EX-Z55" },
    {  9313536,2858,2172, 0, 0,14,30, 0,0x94,7,1,"Casio","EX-P600" },
    { 10834368,3114,2319, 0, 0,27, 0, 0,0x94,0,1,"Casio","EX-Z750" },
    { 10843712,3114,2321, 0, 0,25, 0, 0,0x94,0,1,"Casio","EX-Z75" },
    { 10979200,3114,2350, 0, 0,32,32, 0,0x94,7,1,"Casio","EX-P700" },
    { 12310144,3285,2498, 0, 0, 6,30, 0,0x94,0,1,"Casio","EX-Z850" },
    { 12489984,3328,2502, 0, 0,47,35, 0,0x94,0,1,"Casio","EX-Z8" },
    { 15499264,3754,2752, 0, 0,82, 0, 0,0x94,0,1,"Casio","EX-Z1050" },
    { 18702336,4096,3044, 0, 0,24, 0,80,0x94,7,1,"Casio","EX-ZR100" },
    {  7684000,2260,1700, 0, 0, 0, 0,13,0x94,0,1,"Casio","QV-4000" },
    {   787456,1024, 769, 0, 1, 0, 0, 0,0x49,0,0,"Creative","PC-CAM 600" },
    { 28829184,4384,3288, 0, 0, 0, 0,36,0x61,0,0,"DJI" },
    { 15151104,4608,3288, 0, 0, 0, 0, 0,0x94,0,0,"Matrix" },
    {  3840000,1600,1200, 0, 0, 0, 0,65,0x49,0,0,"Foculus","531C" },
    {   307200, 640, 480, 0, 0, 0, 0, 0,0x94,0,0,"Generic","640x480" },
    {    62464, 256, 244, 1, 1, 6, 1, 0,0x8d,0,0,"Kodak","DC20" },
    {   124928, 512, 244, 1, 1,10, 1, 0,0x8d,0,0,"Kodak","DC20" },
    {  1652736,1536,1076, 0,52, 0, 0, 0,0x61,0,0,"Kodak","DCS200" },
    {  4159302,2338,1779, 1,33, 1, 2, 0,0x94,0,0,"Kodak","C330" },
    {  4162462,2338,1779, 1,33, 1, 2, 0,0x94,0,0,"Kodak","C330",3160 },
    {  6163328,2864,2152, 0, 0, 0, 0, 0,0x94,0,0,"Kodak","C603" },
    {  6166488,2864,2152, 0, 0, 0, 0, 0,0x94,0,0,"Kodak","C603",3160 },
    {   460800, 640, 480, 0, 0, 0, 0, 0,0x00,0,0,"Kodak","C603" },
    {  9116448,2848,2134, 0, 0, 0, 0, 0,0x00,0,0,"Kodak","C603" },
    { 12241200,4040,3030, 2, 0, 0,13, 0,0x49,0,0,"Kodak","12MP" },
    { 12272756,4040,3030, 2, 0, 0,13, 0,0x49,0,0,"Kodak","12MP",31556 },
    { 18000000,4000,3000, 0, 0, 0, 0, 0,0x00,0,0,"Kodak","12MP" },
    {   614400, 640, 480, 0, 3, 0, 0,64,0x94,0,0,"Kodak","KAI-0340" },
    {  3884928,1608,1207, 0, 0, 0, 0,96,0x16,0,0,"Micron","2010",3212 },
    {  1138688,1534, 986, 0, 0, 0, 0, 0,0x61,0,0,"Minolta","RD175",513 },
    {  1581060,1305, 969, 0, 0,18, 6, 6,0x1e,4,1,"Nikon","E900" },
    {  2465792,1638,1204, 0, 0,22, 1, 6,0x4b,5,1,"Nikon","E950" },
    {  2940928,1616,1213, 0, 0, 0, 7,30,0x94,0,1,"Nikon","E2100" },
    {  4771840,2064,1541, 0, 0, 0, 1, 6,0xe1,0,1,"Nikon","E990" },
    {  4775936,2064,1542, 0, 0, 0, 0,30,0x94,0,1,"Nikon","E3700" },
    {  5865472,2288,1709, 0, 0, 0, 1, 6,0xb4,0,1,"Nikon","E4500" },
    {  5869568,2288,1710, 0, 0, 0, 0, 6,0x16,0,1,"Nikon","E4300" },
    {  7438336,2576,1925, 0, 0, 0, 1, 6,0xb4,0,1,"Nikon","E5000" },
    {  8998912,2832,2118, 0, 0, 0, 0,30,0x94,7,1,"Nikon","COOLPIX S6" },
    {  5939200,2304,1718, 0, 0, 0, 0,30,0x16,0,0,"Olympus","C770UZ" },
    {  3178560,2064,1540, 0, 0, 0, 0, 0,0x94,0,1,"Pentax","Optio S" },
    {  4841984,2090,1544, 0, 0,22, 0, 0,0x94,7,1,"Pentax","Optio S" },
    {  6114240,2346,1737, 0, 0,22, 0, 0,0x94,7,1,"Pentax","Optio S4" },
    { 10702848,3072,2322, 0, 0, 0,21,30,0x94,0,1,"Pentax","Optio 750Z" },
    { 13248000,2208,3000, 0, 0, 0, 0,13,0x61,0,0,"Pixelink","A782" },
    {  6291456,2048,1536, 0, 0, 0, 0,96,0x61,0,0,"RoverShot","3320AF" },
    {   311696, 644, 484, 0, 0, 0, 0, 0,0x16,0,8,"ST Micro","STV680 VGA" },
    { 16098048,3288,2448, 0, 0,24, 0, 9,0x94,0,1,"Samsung","S85" },
    { 16215552,3312,2448, 0, 0,48, 0, 9,0x94,0,1,"Samsung","S85" },
    { 20487168,3648,2808, 0, 0, 0, 0,13,0x94,5,1,"Samsung","WB550" },
    { 24000000,4000,3000, 0, 0, 0, 0,13,0x94,5,1,"Samsung","WB550" },
    { 12582980,3072,2048, 0, 0, 0, 0,33,0x61,0,0,"Sinar","3072x2048",68 },
    { 33292868,4080,4080, 0, 0, 0, 0,33,0x61,0,0,"Sinar","4080x4080",68 },
    { 44390468,4080,5440, 0, 0, 0, 0,33,0x61,0,0,"Sinar","4080x5440",68 },
    {  1409024,1376,1024, 0, 0, 1, 0, 0,0x49,0,0,"Sony","XCD-SX910CR" },
    {  2818048,1376,1024, 0, 0, 1, 0,97,0x49,0,0,"Sony","XCD-SX910CR" },
  };
  static const char *corp[] =
    { "AgfaPhoto", "Canon", "Casio", "Epson", "Fujifilm",
      "Mamiya", "Minolta", "Motorola", "Kodak", "Konica", "Leica",
      "Nikon", "Nokia", "Olympus", "Pentax", "Phase One", "Ricoh",
      "Samsung", "Sigma", "Sinar", "Sony" };
  char head[32], *cp;
  int hlen, flen, fsize, zero_fsize=1, i, c;
  struct jhead jh;

  tiff_flip = flip = filters = UINT_MAX;	/* unknown */
  raw_height = raw_width = fuji_width = fuji_layout = cr2_slice[0] = 0;
  maximum = height = width = top_margin = left_margin = 0;
  cdesc[0] = desc[0] = artist[0] = make[0] = model[0] = model2[0] = 0;
  iso_speed = shutter = aperture = focal_len = unique_id = 0;
  tiff_nifds = 0;
  memset (tiff_ifd, 0, sizeof tiff_ifd);
  memset (gpsdata, 0, sizeof gpsdata);
  memset (cblack, 0, sizeof cblack);
  memset (white, 0, sizeof white);
  memset (mask, 0, sizeof mask);
  thumb_offset = thumb_length = thumb_width = thumb_height = 0;
  load_raw = thumb_load_raw = 0;
  write_thumb = &CLASS jpeg_thumb;
  data_offset = meta_length = tiff_bps = tiff_compress = 0;
  kodak_cbpp = zero_after_ff = dng_version = load_flags = 0;
  timestamp = shot_order = tiff_samples = black = is_foveon = 0;
  mix_green = profile_length = data_error = zero_is_bad = 0;
  pixel_aspect = is_raw = raw_color = 1;
  tile_width = tile_length = 0;
  for (i=0; i < 4; i++) {
    cam_mul[i] = i == 1;
    pre_mul[i] = i < 3;
    FORC3 cmatrix[c][i] = 0;
    FORC3 rgb_cam[c][i] = c == i;
  }
  colors = 3;
  for (i=0; i < 0x10000; i++) curve[i] = i;

  order = get2();
  hlen = get4();
  fseek (ifp, 0, SEEK_SET);
  fread (head, 1, 32, ifp);
  fseek (ifp, 0, SEEK_END);
  flen = fsize = ftell(ifp);
  if ((cp = (char *) memmem (head, 32, (char*)"MMMM", 4)) ||
      (cp = (char *) memmem (head, 32, (char*)"IIII", 4))) {
    parse_phase_one (cp-head);
    if (cp-head && parse_tiff(0)) apply_tiff();
  } else if (order == 0x4949 || order == 0x4d4d) {
    if (!memcmp (head+6,"HEAPCCDR",8)) {
      data_offset = hlen;
      parse_ciff (hlen, flen-hlen, 0);
      load_raw = &CLASS canon_load_raw;
    } else if (parse_tiff(0)) apply_tiff();
  } else if (!memcmp (head,"\xff\xd8\xff\xe1",4) &&
	     !memcmp (head+6,"Exif",4)) {
    fseek (ifp, 4, SEEK_SET);
    data_offset = 4 + get2();
    fseek (ifp, data_offset, SEEK_SET);
    if (fgetc(ifp) != 0xff)
      parse_tiff(12);
    thumb_offset = 0;
  } else if (!memcmp (head+25,"ARECOYK",7)) {
    strcpy (make, "Contax");
    strcpy (model,"N Digital");
    fseek (ifp, 33, SEEK_SET);
    get_timestamp(1);
    fseek (ifp, 60, SEEK_SET);
    FORC4 cam_mul[c ^ (c >> 1)] = get4();
  } else if (!strcmp (head, "PXN")) {
    strcpy (make, "Logitech");
    strcpy (model,"Fotoman Pixtura");
  } else if (!strcmp (head, "qktk")) {
    strcpy (make, "Apple");
    strcpy (model,"QuickTake 100");
    load_raw = &CLASS quicktake_100_load_raw;
  } else if (!strcmp (head, "qktn")) {
    strcpy (make, "Apple");
    strcpy (model,"QuickTake 150");
    load_raw = &CLASS kodak_radc_load_raw;
  } else if (!memcmp (head,"FUJIFILM",8)) {
    fseek (ifp, 84, SEEK_SET);
    thumb_offset = get4();
    thumb_length = get4();
    fseek (ifp, 92, SEEK_SET);
    parse_fuji (get4());
    if (thumb_offset > 120) {
      fseek (ifp, 120, SEEK_SET);
      is_raw += (i = get4()) && 1;
      if (is_raw == 2 && shot_select)
	parse_fuji (i);
    }
    load_raw = &CLASS unpacked_load_raw;
    fseek (ifp, 100+28*(shot_select > 0), SEEK_SET);
    parse_tiff (data_offset = get4());
    parse_tiff (thumb_offset+12);
    apply_tiff();
  } else if (!memcmp (head,"RIFF",4)) {
    fseek (ifp, 0, SEEK_SET);
    parse_riff();
  } else if (!memcmp (head,"\0\001\0\001\0@",6)) {
    fseek (ifp, 6, SEEK_SET);
    fread (make, 1, 8, ifp);
    fread (model, 1, 8, ifp);
    fread (model2, 1, 16, ifp);
    data_offset = get2();
    get2();
    raw_width = get2();
    raw_height = get2();
    load_raw = &CLASS nokia_load_raw;
    filters = 0x61616161;
  } else if (!memcmp (head,"NOKIARAW",8)) {
    strcpy (make, "NOKIA");
    order = 0x4949;
    fseek (ifp, 300, SEEK_SET);
    data_offset = get4();
    i = get4();
    width = get2();
    height = get2();
    switch (tiff_bps = i*8 / (width * height)) {
      case  8: load_raw = &CLASS eight_bit_load_raw;  break;
      case 10: load_raw = &CLASS nokia_load_raw;
    }
    raw_height = height + (top_margin = i / (width * tiff_bps/8) - height);
    mask[0][3] = 1;
    filters = 0x61616161;
  } else if (!memcmp (head,"ARRI",4)) {
    order = 0x4949;
    fseek (ifp, 20, SEEK_SET);
    width = get4();
    height = get4();
    strcpy (make, "ARRI");
    fseek (ifp, 668, SEEK_SET);
    fread (model, 1, 64, ifp);
    data_offset = 4096;
    load_raw = &CLASS packed_load_raw;
    load_flags = 88;
    filters = 0x61616161;
  } else if (!memcmp (head,"XPDS",4)) {
    order = 0x4949;
    fseek (ifp, 0x800, SEEK_SET);
    fread (make, 1, 41, ifp);
    raw_height = get2();
    raw_width  = get2();
    fseek (ifp, 56, SEEK_CUR);
    fread (model, 1, 30, ifp);
    data_offset = 0x10000;
    load_raw = &CLASS canon_rmf_load_raw;
    gamma_curve (0, 12.25, 1, 1023);
  } else if (!memcmp (head+4,"RED1",4)) {
    strcpy (make, "Red");
    strcpy (model,"One");
    parse_redcine();
    load_raw = &CLASS redcine_load_raw;
    gamma_curve (1/2.4, 12.92, 1, 4095);
    filters = 0x49494949;
  } else if (!memcmp (head,"DSC-Image",9))
    parse_rollei();
  else if (!memcmp (head,"PWAD",4))
    parse_sinar_ia();
  else if (!memcmp (head,"\0MRM",4))
    parse_minolta(0);
  else if (!memcmp (head,"FOVb",4))
    {
#ifdef LIBRAW_LIBRARY_BUILD
#ifdef  LIBRAW_DEMOSAIC_PACK_GPL2
      if(!imgdata.params.force_foveon_x3f)
        parse_foveon();
      else
#endif
        parse_x3f();
#else
#ifdef  LIBRAW_DEMOSAIC_PACK_GPL2
      parse_foveon();
#endif
#endif
    }
  else if (!memcmp (head,"CI",2))
    parse_cine();
  else
    for (zero_fsize=i=0; i < sizeof table / sizeof *table; i++)
      if (fsize == table[i].fsize) {
	strcpy (make,  table[i].t_make );
	strcpy (model, table[i].t_model);
	flip = table[i].flags >> 2;
	zero_is_bad = table[i].flags & 2;
	if (table[i].flags & 1)
	  parse_external_jpeg();
	data_offset = table[i].offset;
	raw_width   = table[i].rw;
	raw_height  = table[i].rh;
	left_margin = table[i].lm;
	 top_margin = table[i].tm;
	width  = raw_width - left_margin - table[i].rm;
	height = raw_height - top_margin - table[i].bm;
	filters = 0x1010101 * table[i].cf;
	colors = 4 - !((filters & filters >> 1) & 0x5555);
	load_flags = table[i].lf;
	switch (tiff_bps = (fsize-data_offset)*8 / (raw_width*raw_height)) {
	  case 6:
	    load_raw = &CLASS minolta_rd175_load_raw;  break;
	  case 8:
	    load_raw = &CLASS eight_bit_load_raw;  break;
	  case 10: case 12:
	    load_flags |= 128;
	    load_raw = &CLASS packed_load_raw;     break;
	  case 16:
	    order = 0x4949 | 0x404 * (load_flags & 1);
	    tiff_bps -= load_flags >> 4;
	    tiff_bps -= load_flags = load_flags >> 1 & 7;
	    load_raw = &CLASS unpacked_load_raw;
	}
	maximum = (1 << tiff_bps) - (1 << table[i].max);
      }
  if (zero_fsize) fsize = 0;
  if (make[0] == 0) parse_smal (0, flen);
  if (make[0] == 0) {
    parse_jpeg(0);
    fseek(ifp,0,SEEK_END);
    int sz = ftell(ifp);
    if (!strncmp(model,"ov",2) && sz>=6404096 && !fseek (ifp, -6404096, SEEK_END) &&
	fread (head, 1, 32, ifp) && !strcmp(head,"BRCMn")) {
      strcpy (make, "OmniVision");
      data_offset = ftell(ifp) + 0x8000-32;
      width = raw_width;
      raw_width = 2611;
      load_raw = &CLASS nokia_load_raw;
      filters = 0x16161616;
    } else is_raw = 0;
  }

  for (i=0; i < sizeof corp / sizeof *corp; i++)
    if (strcasestr (make, corp[i]))	/* Simplify company names */
	    strcpy (make, corp[i]);
  if ((!strcmp(make,"Kodak") || !strcmp(make,"Leica")) &&
	((cp = strcasestr(model," DIGITAL CAMERA")) ||
	 (cp = strstr(model,"FILE VERSION"))))
     *cp = 0;
  if (!strncasecmp(model,"PENTAX",6))
    strcpy (make, "Pentax");
  cp = make + strlen(make);		/* Remove trailing spaces */
  while (*--cp == ' ') *cp = 0;
  cp = model + strlen(model);
  while (*--cp == ' ') *cp = 0;
  i = strlen(make);			/* Remove make from model */
  if (!strncasecmp (model, make, i) && model[i++] == ' ')
    memmove (model, model+i, 64-i);
  if (!strncmp (model,"FinePix ",8))
    strcpy (model, model+8);
  if (!strncmp (model,"Digital Camera ",15))
    strcpy (model, model+15);
  desc[511] = artist[63] = make[63] = model[63] = model2[63] = 0;
  if (!is_raw) goto notraw;

  if (!height) height = raw_height;
  if (!width)  width  = raw_width;
  if (height == 2624 && width == 3936)	/* Pentax K10D and Samsung GX10 */
    { height  = 2616;   width  = 3896; }
  if (height == 3136 && width == 4864)  /* Pentax K20D and Samsung GX20 */
    { height  = 3124;   width  = 4688; filters = 0x16161616; }
  if (width == 4352 && (!strcmp(model,"K-r") || !strcmp(model,"K-x")))
    {			width  = 4309; filters = 0x16161616; }
  if (width >= 4960 && !strncmp(model,"K-5",3))
    { left_margin = 10; width  = 4950; filters = 0x16161616; }
  if (width == 4736 && !strcmp(model,"K-7"))
    { height  = 3122;   width  = 4684; filters = 0x16161616; top_margin = 2; }
  if (width == 6080 && !strcmp(model,"K-3"))
    { left_margin = 4;  width  = 6040; }
  if (width == 7424 && !strcmp(model,"645D"))
    { height  = 5502;   width  = 7328; filters = 0x61616161; top_margin = 29;
      left_margin = 48; }
  if (height == 3014 && width == 4096)	/* Ricoh GX200 */
			width  = 4014;
  if (dng_version) {
    if (filters == UINT_MAX) filters = 0;
    if (filters) is_raw = tiff_samples;
    else	 colors = tiff_samples;
    switch (tiff_compress) {
    case 0:  /* Compression not set, assuming uncompressed */
      case 1:     load_raw = &CLASS   packed_dng_load_raw;  break;
      case 7:     load_raw = &CLASS lossless_dng_load_raw;  break;
      case 34892: load_raw = &CLASS    lossy_dng_load_raw;  break;
      default:    load_raw = 0;
    }
    goto dng_skip;
  }
  if (!strcmp(make,"Canon") && !fsize && tiff_bps != 15) {
    if (!load_raw)
      load_raw = &CLASS lossless_jpeg_load_raw;
    for (i=0; i < sizeof canon / sizeof *canon; i++)
      if (raw_width == canon[i][0] && raw_height == canon[i][1]) {
	width  = raw_width - (left_margin = canon[i][2]);
	height = raw_height - (top_margin = canon[i][3]);
	width  -= canon[i][4];
	height -= canon[i][5];
	mask[0][1] =  canon[i][6];
	mask[0][3] = -canon[i][7];
	mask[1][1] =  canon[i][8];
	mask[1][3] = -canon[i][9];
	if (canon[i][10]) filters = canon[i][10] * 0x01010101;
      }
    if ((unique_id | 0x20000) == 0x2720000) {
      left_margin = 8;
      top_margin = 16;
    }
  }
  if (!strcmp(make,"Canon") && unique_id)
    {
      for (i=0; i < sizeof unique / sizeof *unique; i++)
        if (unique_id == 0x80000000 + unique[i].id)
          {
            adobe_coeff ("Canon", unique[i].t_model);
            strcpy(model,unique[i].t_model);
          }
    }

  if (!strcasecmp(make,"Sony") && unique_id)
    {
      for (i=0; i < sizeof sony_unique / sizeof *sony_unique; i++)
        if (unique_id == sony_unique[i].id)
          {
            adobe_coeff ("Sony", sony_unique[i].t_model);
            strcpy(model,sony_unique[i].t_model);
          }
    }

  if (!strcmp(make,"Nikon")) {
    if (!load_raw)
      load_raw = &CLASS packed_load_raw;
    if (model[0] == 'E')
      load_flags |= !data_offset << 2 | 2;
  }

/* Set parameters based on camera name (for non-DNG files). */

  if (!strcmp(model,"KAI-0340")
	&& find_green (16, 16, 3840, 5120) < 25) {
    height = 480;
    top_margin = filters = 0;
    strcpy (model,"C603");
  }
  if (is_foveon) {
    if (height*2 < width) pixel_aspect = 0.5;
    if (height   > width) pixel_aspect = 2;
    filters = 0;
#ifdef LIBRAW_DEMOSAIC_PACK_GPL2
    if(!imgdata.params.force_foveon_x3f)
      simple_coeff(0);
#endif
  } else if (!strcmp(make,"Canon") && tiff_bps == 15) {
    switch (width) {
      case 3344: width -= 66;
      case 3872: width -= 6;
    }
    if (height > width) SWAP(height,width);
    filters = 0;
    tiff_samples = colors = 3;
    load_raw = &CLASS canon_sraw_load_raw;
  } else if (!strcmp(model,"PowerShot 600")) {
    height = 613;
    width  = 854;
    raw_width = 896;
    colors = 4;
    filters = 0xe1e4e1e4;
    load_raw = &CLASS canon_600_load_raw;
  } else if (!strcmp(model,"PowerShot A5") ||
	     !strcmp(model,"PowerShot A5 Zoom")) {
    height = 773;
    width  = 960;
    raw_width = 992;
    pixel_aspect = 256/235.0;
    filters = 0x1e4e1e4e;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot A50")) {
    height =  968;
    width  = 1290;
    raw_width = 1320;
    filters = 0x1b4e4b1e;
    goto canon_a5;
  } else if (!strcmp(model,"PowerShot Pro70")) {
    height = 1024;
    width  = 1552;
    filters = 0x1e4b4e1b;
canon_a5:
    colors = 4;
    tiff_bps = 10;
    load_raw = &CLASS packed_load_raw;
    load_flags = 40;
  } else if (!strcmp(model,"PowerShot Pro90 IS") ||
	     !strcmp(model,"PowerShot G1")) {
    colors = 4;
    filters = 0xb4b4b4b4;
  } else if (!strcmp(model,"PowerShot A610")) {
    if (canon_s2is()) strcpy (model+10, "S2 IS");
  } else if (!strcmp(model,"PowerShot SX220 HS")) {
    mask[1][3] = -4;
  } else if (!strcmp(model,"EOS D2000C")) {
    filters = 0x61616161;
    black = curve[200];
  } else if (!strcmp(model,"D1")) {
    cam_mul[0] *= 256/527.0;
    cam_mul[2] *= 256/317.0;
  } else if (!strcmp(model,"D1X")) {
    width -= 4;
    pixel_aspect = 0.5;
  } else if (!strcmp(model,"D40X") ||
	     !strcmp(model,"D60")  ||
	     !strcmp(model,"D80")  ||
	     !strcmp(model,"D3000")) {
    height -= 3;
    width  -= 4;
  } else if (!strcmp(model,"D3")   ||
	     !strcmp(model,"D3S")  ||
	     !strcmp(model,"D700")) {
    width -= 4;
    left_margin = 2;
  } else if (!strcmp(model,"D3100")) {
    width -= 28;
    left_margin = 6;
  } else if (!strcmp(model,"D5000") ||
	     !strcmp(model,"D90")) {
    width -= 42;
  } else if (!strcmp(model,"D5100") ||
	     !strcmp(model,"D7000") ||
	     !strcmp(model,"COOLPIX A")) {
    width -= 44;
  } else if (!strcmp(model,"D3200") ||
	     !strcmp(model,"D600")  ||
	     !strcmp(model,"D610")  ||
	    !strncmp(model,"D800",4)) {
    width -= 46;
  } else if (!strcmp(model,"D4") ||
	     !strcmp(model,"Df")) {
    width -= 52;
    left_margin = 2;
  } else if (!strncmp(model,"D40",3) ||
	     !strncmp(model,"D50",3) ||
	     !strncmp(model,"D70",3)) {
    width--;
  } else if (!strcmp(model,"D100")) {
    if (load_flags)
      raw_width = (width += 3) + 3;
  } else if (!strcmp(model,"D200")) {
    left_margin = 1;
    width -= 4;
    filters = 0x94949494;
  } else if (!strncmp(model,"D2H",3)) {
    left_margin = 6;
    width -= 14;
  } else if (!strncmp(model,"D2X",3)) {
    if (width == 3264) width -= 32;
    else width -= 8;
  } else if (!strncmp(model,"D300",4)) {
    width -= 32;
  } else if (!strcmp(make,"Nikon") && raw_width == 4032) {
    adobe_coeff ("Nikon","COOLPIX P7700");
  } else if (!strncmp(model,"COOLPIX P",9)) {
    load_flags = 24;
    filters = 0x94949494;
    if (model[9] == '7' && iso_speed >= 400)
      black = 255;
  } else if (!strncmp(model,"1 ",2)) {
    height -= 2;
  } else if (fsize == 1581060) {
    simple_coeff(3);
    pre_mul[0] = 1.2085;
    pre_mul[1] = 1.0943;
    pre_mul[3] = 1.1103;
  } else if (fsize == 3178560) {
    cam_mul[0] *= 4;
    cam_mul[2] *= 4;
  } else if (fsize == 4771840) {
    if (!timestamp && nikon_e995())
      strcpy (model, "E995");
    if (strcmp(model,"E995")) {
      filters = 0xb4b4b4b4;
      simple_coeff(3);
      pre_mul[0] = 1.196;
      pre_mul[1] = 1.246;
      pre_mul[2] = 1.018;
    }
  } else if (fsize == 2940928) {
    if (!timestamp && !nikon_e2100())
      strcpy (model,"E2500");
    if (!strcmp(model,"E2500")) {
      height -= 2;
      load_flags = 6;
      colors = 4;
      filters = 0x4b4b4b4b;
    }
  } else if (fsize == 4775936) {
    if (!timestamp) nikon_3700();
    if (model[0] == 'E' && atoi(model+1) < 3700)
      filters = 0x49494949;
    if (!strcmp(model,"Optio 33WR")) {
      flip = 1;
      filters = 0x16161616;
    }
    if (make[0] == 'O') {
      i = find_green (12, 32, 1188864, 3576832);
      c = find_green (12, 32, 2383920, 2387016);
      if (abs(i) < abs(c)) {
	SWAP(i,c);
	load_flags = 24;
      }
      if (i < 0) filters = 0x61616161;
    }
  } else if (fsize == 5869568) {
    if (!timestamp && minolta_z2()) {
      strcpy (make, "Minolta");
      strcpy (model,"DiMAGE Z2");
    }
    load_flags = 6 + 24*(make[0] == 'M');
  } else if (fsize == 6291456) {
    fseek (ifp, 0x300000, SEEK_SET);
    if ((order = guess_byte_order(0x10000)) == 0x4d4d) {
      height -= (top_margin = 16);
      width -= (left_margin = 28);
      maximum = 0xf5c0;
      strcpy (make, "ISG");
      model[0] = 0;
    }
  } else if (!strcmp(make,"Fujifilm")) {
    if (!strcmp(model+7,"S2Pro")) {
      strcpy (model,"S2Pro");
      height = 2144;
      width  = 2880;
      flip = 6;
    } else if (load_raw != &CLASS packed_load_raw)
      maximum = (is_raw == 2 && shot_select) ? 0x2f00 : 0x3e00;
    top_margin = (raw_height - height) >> 2 << 1;
    left_margin = (raw_width - width ) >> 2 << 1;
    if (width == 2848 || width == 3664) filters = 0x16161616;
    if (width == 4032 || width == 4952) left_margin = 0;
    if (width == 3328 && (width -= 66)) left_margin = 34;
    if (width == 4936) left_margin = 4;
    if (!strcmp(model,"HS50EXR") ||
	!strcmp(model,"F900EXR")) {
      width += 2;
      left_margin = 0;
      filters = 0x16161616;
    }
    if (fuji_layout) raw_width *= is_raw;
  } else if (!strcmp(model,"KD-400Z")) {
    height = 1712;
    width  = 2312;
    raw_width = 2336;
    goto konica_400z;
  } else if (!strcmp(model,"KD-510Z")) {
    goto konica_510z;
  } else if (!strcasecmp(make,"Minolta")) {
    if (!load_raw && (maximum = 0xfff))
      load_raw = &CLASS unpacked_load_raw;
    if (!strncmp(model,"DiMAGE A",8)) {
      if (!strcmp(model,"DiMAGE A200"))
	filters = 0x49494949;
      tiff_bps = 12;
      load_raw = &CLASS packed_load_raw;
    } else if (!strncmp(model,"ALPHA",5) ||
	       !strncmp(model,"DYNAX",5) ||
	       !strncmp(model,"MAXXUM",6)) {
      sprintf (model+20, "DYNAX %-10s", model+6+(model[0]=='M'));
      adobe_coeff (make, model+20);
      load_raw = &CLASS packed_load_raw;
    } else if (!strncmp(model,"DiMAGE G",8)) {
      if (model[8] == '4') {
	height = 1716;
	width  = 2304;
      } else if (model[8] == '5') {
konica_510z:
	height = 1956;
	width  = 2607;
	raw_width = 2624;
      } else if (model[8] == '6') {
	height = 2136;
	width  = 2848;
      }
      data_offset += 14;
      filters = 0x61616161;
konica_400z:
      load_raw = &CLASS unpacked_load_raw;
      maximum = 0x3df;
      order = 0x4d4d;
    }
  } else if (!strcmp(model,"*ist D")) {
    load_raw = &CLASS unpacked_load_raw;
    data_error = -1;
  } else if (!strcmp(model,"*ist DS")) {
    height -= 2;
  } else if (!strcmp(make,"Samsung") && raw_width == 4704) {
    height -= top_margin = 8;
    width -= 2 * (left_margin = 8);
    load_flags = 32;
  } else if (!strcmp(make,"Samsung") && raw_height == 3714) {
    height -= 18;
    width = 5536;
    filters = 0x61616161;
    colors = 3;
  } else if (!strcmp(make,"Samsung") && raw_width == 5632) {
    order = 0x4949;
    height = 3694;
    top_margin = 2;
    width  = 5574 - (left_margin = 32 + tiff_bps);
    if (tiff_bps == 12) load_flags = 80;
  } else if (!strcmp(model,"EX1")) {
    order = 0x4949;
    height -= 20;
    top_margin = 2;
    if ((width -= 6) > 3682) {
      height -= 10;
      width  -= 46;
      top_margin = 8;
    }
  } else if (!strcmp(model,"WB2000")) {
    order = 0x4949;
    height -= 3;
    top_margin = 2;
    if ((width -= 10) > 3718) {
      height -= 28;
      width  -= 56;
      top_margin = 8;
    }
  } else if (strstr(model,"WB550")) {
    strcpy (model, "WB550");
  } else if (!strcmp(model,"EX2F")) {
    height = 3045;
    width  = 4070;
    top_margin = 3;
    order = 0x4949;
    filters = 0x49494949;
    load_raw = &CLASS unpacked_load_raw;
  } else if (!strcmp(model,"STV680 VGA")) {
    black = 16;
  } else if (!strcmp(model,"N95")) {
    height = raw_height - (top_margin = 2);
  } else if (!strcmp(model,"640x480")) {
    gamma_curve (0.45, 4.5, 1, 255);
  } else if (!strcmp(make,"Hasselblad")) {
    if (load_raw == &CLASS lossless_jpeg_load_raw)
      load_raw = &CLASS hasselblad_load_raw;
    if (raw_width == 7262) {
      height = 5444;
      width  = 7248;
      top_margin  = 4;
      left_margin = 7;
      filters = 0x61616161;
    } else if (raw_width == 7410) {
      height = 5502;
      width  = 7328;
      top_margin  = 4;
      left_margin = 41;
      filters = 0x61616161;
    } else if (raw_width == 9044) {
      height = 6716;
      width  = 8964;
      top_margin  = 8;
      left_margin = 40;
      black += load_flags = 256;
      maximum = 0x8101;
    } else if (raw_width == 4090) {
      strcpy (model, "V96C");
      height -= (top_margin = 6);
      width -= (left_margin = 3) + 7;
      filters = 0x61616161;
    }
  } else if (!strcmp(make,"Sinar")) {
    if (!load_raw) load_raw = &CLASS unpacked_load_raw;
    maximum = 0x3fff;
  } else if (!strcmp(make,"Leaf")) {
    maximum = 0x3fff;
    fseek (ifp, data_offset, SEEK_SET);
    if (ljpeg_start (&jh, 1) && jh.bits == 15)
      maximum = 0x1fff;
    if (tiff_samples > 1) filters = 0;
    if (tiff_samples > 1 || tile_length < raw_height) {
      load_raw = &CLASS leaf_hdr_load_raw;
      raw_width = tile_width;
    }
    if ((width | height) == 2048) {
      if (tiff_samples == 1) {
	filters = 1;
	strcpy (cdesc, "RBTG");
	strcpy (model, "CatchLight");
	top_margin =  8; left_margin = 18; height = 2032; width = 2016;
      } else {
	strcpy (model, "DCB2");
	top_margin = 10; left_margin = 16; height = 2028; width = 2022;
      }
    } else if (width+height == 3144+2060) {
      if (!model[0]) strcpy (model, "Cantare");
      if (width > height) {
	 top_margin = 6; left_margin = 32; height = 2048;  width = 3072;
	filters = 0x61616161;
      } else {
	left_margin = 6;  top_margin = 32;  width = 2048; height = 3072;
	filters = 0x16161616;
      }
      if (!cam_mul[0] || model[0] == 'V') filters = 0;
      else is_raw = tiff_samples;
    } else if (width == 2116) {
      strcpy (model, "Valeo 6");
      height -= 2 * (top_margin = 30);
      width -= 2 * (left_margin = 55);
      filters = 0x49494949;
    } else if (width == 3171) {
      strcpy (model, "Valeo 6");
      height -= 2 * (top_margin = 24);
      width -= 2 * (left_margin = 24);
      filters = 0x16161616;
    }
  } else if (!strcmp(make,"Leica") || !strcmp(make,"Panasonic")) {
    if ((flen - data_offset) / (raw_width*8/7) == raw_height)
      load_raw = &CLASS panasonic_load_raw;
    if (!load_raw) {
      load_raw = &CLASS unpacked_load_raw;
      load_flags = 4;
    }
    zero_is_bad = 1;
    if ((height += 12) > raw_height) height = raw_height;
    for (i=0; i < sizeof pana / sizeof *pana; i++)
      if (raw_width == pana[i][0] && raw_height == pana[i][1]) {
	left_margin = pana[i][2];
	 top_margin = pana[i][3];
	     width += pana[i][4];
	    height += pana[i][5];
      }
    filters = 0x01010101 * (uchar) "\x94\x61\x49\x16"
	[((filters-1) ^ (left_margin & 1) ^ (top_margin << 1)) & 3];
  } else if (!strcmp(model,"C770UZ")) {
    height = 1718;
    width  = 2304;
    filters = 0x16161616;
    load_raw = &CLASS packed_load_raw;
    load_flags = 30;
  } else if (!strcmp(make,"Olympus")) {
    height += height & 1;
    if (exif_cfa) filters = exif_cfa;
    if (width == 4100) width -= 4;
    if (width == 4080) width -= 24;
    if (load_raw == &CLASS unpacked_load_raw)
      load_flags = 4;
    tiff_bps = 12;
    if (!strcmp(model,"E-300") ||
	!strcmp(model,"E-500")) {
      width -= 20;
      if (load_raw == &CLASS unpacked_load_raw) {
	maximum = 0xfc3;
	memset (cblack, 0, sizeof cblack);
      }
    } else if (!strcmp(model,"STYLUS1")) {
      width -= 14;
      maximum = 0xfff;
    } else if (!strcmp(model,"E-330")) {
      width -= 30;
      if (load_raw == &CLASS unpacked_load_raw)
	maximum = 0xf79;
    } else if (!strcmp(model,"SP550UZ")) {
      thumb_length = flen - (thumb_offset = 0xa39800);
      thumb_height = 480;
      thumb_width  = 640;
    }
  } else if (!strcmp(model,"N Digital")) {
    height = 2047;
    width  = 3072;
    filters = 0x61616161;
    data_offset = 0x1a00;
    load_raw = &CLASS packed_load_raw;
  } else if (!strcmp(model,"DSC-F828")) {
    width = 3288;
    left_margin = 5;
    mask[1][3] = -17;
    data_offset = 862144;
    load_raw = &CLASS sony_load_raw;
    filters = 0x9c9c9c9c;
    colors = 4;
    strcpy (cdesc, "RGBE");
  } else if (!strcmp(model,"DSC-V3")) {
    width = 3109;
    left_margin = 59;
    mask[0][1] = 9;
    data_offset = 787392;
    load_raw = &CLASS sony_load_raw;
  } else if (!strcmp(make,"Sony") && raw_width == 3984) {
    adobe_coeff ("Sony","DSC-R1");
    width = 3925;
    order = 0x4d4d;
  } else if (!strcmp(make,"Sony") && !strcmp(model,"ILCE-3000")) {
    width -= 32;
  } else if (!strcmp(make,"Sony") && raw_width == 5504) {
    width -= 8;
  } else if (!strcmp(make,"Sony") && raw_width == 6048) {
    width -= 24;
  } else if (!strcmp(make,"Sony") && raw_width == 7392) {
    width -= 24; // 21 pix really
  } else if (!strcmp(model,"DSLR-A100")) {
    if (width == 3880) {
      height--;
      width = ++raw_width;
    } else {
      order = 0x4d4d;
      load_flags = 2;
    }
    filters = 0x61616161;
  } else if (!strcmp(model,"DSLR-A350")) {
    height -= 4;
  } else if (!strcmp(model,"PIXL")) {
    height -= top_margin = 4;
    width -= left_margin = 32;
    gamma_curve (0, 7, 1, 255);
  } else if (!strcmp(model,"C603") || !strcmp(model,"C330")
	|| !strcmp(model,"12MP")) {
    order = 0x4949;
    if (filters && data_offset) {
      fseek (ifp, data_offset < 4096 ? 168 : 5252, SEEK_SET);
      read_shorts (curve, 256);
    } else gamma_curve (0, 3.875, 1, 255);
    load_raw = filters ? &CLASS eight_bit_load_raw
		       : &CLASS kodak_yrgb_load_raw;
  } else if (!strncasecmp(model,"EasyShare",9)) {
    data_offset = data_offset < 0x15000 ? 0x15000 : 0x17000;
    load_raw = &CLASS packed_load_raw;
  } else if (!strcasecmp(make,"Kodak")) {
    if (filters == UINT_MAX) filters = 0x61616161;
    if (!strncmp(model,"NC2000",6)) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"EOSDCS3B")) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"EOSDCS1")) {
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"DCS420")) {
      width -= 4;
      left_margin = 2;
    } else if (!strncmp(model,"DCS460 ",7)) {
      model[6] = 0;
      width -= 4;
      left_margin = 2;
    } else if (!strcmp(model,"DCS460A")) {
      width -= 4;
      left_margin = 2;
      colors = 1;
      filters = 0;
    } else if (!strcmp(model,"DCS660M")) {
      black = 214;
      colors = 1;
      filters = 0;
    } else if (!strcmp(model,"DCS760M")) {
      colors = 1;
      filters = 0;
    }
    if (!strcmp(model+4,"20X"))
      strcpy (cdesc, "MYCY");
    if (strstr(model,"DC25")) {
      strcpy (model, "DC25");
      data_offset = 15424;
    }
    if (!strncmp(model,"DC2",3)) {
      raw_height = 2 + (height = 242);
      if (flen < 100000) {
	raw_width = 256; width = 249;
	pixel_aspect = (4.0*height) / (3.0*width);
      } else {
	raw_width = 512; width = 501;
	pixel_aspect = (493.0*height) / (373.0*width);
      }
      top_margin = left_margin = 1;
      colors = 4;
      filters = 0x8d8d8d8d;
      simple_coeff(1);
      pre_mul[1] = 1.179;
      pre_mul[2] = 1.209;
      pre_mul[3] = 1.036;
      load_raw = &CLASS eight_bit_load_raw;
    } else if (!strcmp(model,"40")) {
      strcpy (model, "DC40");
      height = 512;
      width  = 768;
      data_offset = 1152;
      load_raw = &CLASS kodak_radc_load_raw;
    } else if (strstr(model,"DC50")) {
      strcpy (model, "DC50");
      height = 512;
      width  = 768;
      data_offset = 19712;
      load_raw = &CLASS kodak_radc_load_raw;
    } else if (strstr(model,"DC120")) {
      strcpy (model, "DC120");
      height = 976;
      width  = 848;
      pixel_aspect = height/0.75/width;
      load_raw = tiff_compress == 7 ?
	&CLASS kodak_jpeg_load_raw : &CLASS kodak_dc120_load_raw;
    } else if (!strcmp(model,"DCS200")) {
      thumb_height = 128;
      thumb_width  = 192;
      thumb_offset = 6144;
      thumb_misc   = 360;
      write_thumb = &CLASS layer_thumb;
      black = 17;
    }
  } else if (!strcmp(model,"Fotoman Pixtura")) {
    height = 512;
    width  = 768;
    data_offset = 3632;
    load_raw = &CLASS kodak_radc_load_raw;
    filters = 0x61616161;
    simple_coeff(2);
  } else if (!strncmp(model,"QuickTake",9)) {
    if (head[5]) strcpy (model+10, "200");
    fseek (ifp, 544, SEEK_SET);
    height = get2();
    width  = get2();
    data_offset = (get4(),get2()) == 30 ? 738:736;
    if (height > width) {
      SWAP(height,width);
      fseek (ifp, data_offset-6, SEEK_SET);
      flip = ~get2() & 3 ? 5:6;
    }
    filters = 0x61616161;
  } else if (!strcmp(make,"Rollei") && !load_raw) {
    switch (raw_width) {
      case 1316:
	height = 1030;
	width  = 1300;
	top_margin  = 1;
	left_margin = 6;
	break;
      case 2568:
	height = 1960;
	width  = 2560;
	top_margin  = 2;
	left_margin = 8;
    }
    filters = 0x16161616;
    load_raw = &CLASS rollei_load_raw;
  }
  else if (!strcmp(model,"GRAS-50S5C")) {
   height = 2048;
   width = 2440;
   load_raw = &CLASS unpacked_load_raw;
   data_offset = 0;
   filters = 0x49494949;
   order = 0x4949;
   maximum = 0xfffC;
  } else if (!strcmp(model,"BB-500CL")) {
   height = 2058;
   width = 2448;
   load_raw = &CLASS unpacked_load_raw;
   data_offset = 0;
   filters = 0x94949494;
   order = 0x4949;
   maximum = 0x3fff;
  } else if (!strcmp(model,"BB-500GE")) {
   height = 2058;
   width = 2456;
   load_raw = &CLASS unpacked_load_raw;
   data_offset = 0;
   filters = 0x94949494;
   order = 0x4949;
   maximum = 0x3fff;
  } else if (!strcmp(model,"SVS625CL")) {
   height = 2050;
   width = 2448;
   load_raw = &CLASS unpacked_load_raw;
   data_offset = 0;
   filters = 0x94949494;
   order = 0x4949;
   maximum = 0x0fff;
  }
  /* Early reject for damaged images */
  if (!load_raw || height < 22 || width < 22 ||
	tiff_bps > 16 || tiff_samples > 4 || colors > 4 || colors < 1)
    {
      is_raw = 0;
#ifdef LIBRAW_LIBRARY_BUILD
      RUN_CALLBACK(LIBRAW_PROGRESS_IDENTIFY,1,2);
#endif
      return;
    }
  if (!model[0])
    sprintf (model, "%dx%d", width, height);
  if (filters == UINT_MAX) filters = 0x94949494;
  if (thumb_offset && !thumb_height) {
    fseek (ifp, thumb_offset, SEEK_SET);
    if (ljpeg_start (&jh, 1)) {
      thumb_width  = jh.wide;
      thumb_height = jh.high;
    }
  }

dng_skip:
  if ((use_camera_matrix & (use_camera_wb || dng_version))
	&& cmatrix[0][0] > 0.125) {
    memcpy (rgb_cam, cmatrix, sizeof cmatrix);
    raw_color = 0;
  }
  if (raw_color) adobe_coeff (make, model);
  if (load_raw == &CLASS kodak_radc_load_raw)
    if (raw_color) adobe_coeff ("Apple","Quicktake");

  if (fuji_width) {
    fuji_width = width >> !fuji_layout;
    if (~fuji_width & 1) filters = 0x49494949;
    width = (height >> fuji_layout) + fuji_width;
    height = width - 1;
    pixel_aspect = 1;
  } else {
    if (raw_height < height) raw_height = height;
    if (raw_width  < width ) raw_width  = width;
  }
  if (!tiff_bps) tiff_bps = 12;
  if (!maximum) maximum = (1 << tiff_bps) - 1;
  if (!load_raw || height < 22 || width < 22 ||
	tiff_bps > 16 || tiff_samples > 4 || colors > 4)
    is_raw = 0;
#ifdef NO_JASPER
  if (load_raw == &CLASS redcine_load_raw) {
#ifdef DCRAW_VERBOSE
    fprintf (stderr,_("%s: You must link dcraw with %s!!\n"),
	ifname, "libjasper");
#endif
    is_raw = 0;
#ifdef LIBRAW_LIBRARY_BUILD
    imgdata.process_warnings |= LIBRAW_WARN_NO_JASPER;
#endif
  }
#endif
#ifdef NO_JPEG
  if (load_raw == &CLASS kodak_jpeg_load_raw ||
      load_raw == &CLASS lossy_dng_load_raw) {
#ifdef DCRAW_VERBOSE
    fprintf (stderr,_("%s: You must link dcraw with %s!!\n"),
	ifname, "libjpeg");
#endif
    is_raw = 0;
#ifdef LIBRAW_LIBRARY_BUILD
    imgdata.process_warnings |= LIBRAW_WARN_NO_JPEGLIB;
#endif
  }
#endif
  if (!cdesc[0])
    strcpy (cdesc, colors == 3 ? "RGBG":"GMCY");
  if (!raw_height) raw_height = height;
  if (!raw_width ) raw_width  = width;
  if (filters > 999 && colors == 3)
    filters |= ((filters >> 2 & 0x22222222) |
		(filters << 2 & 0x88888888)) & filters << 1;
notraw:
  if (flip == UINT_MAX) flip = tiff_flip;
  if (flip == UINT_MAX) flip = 0;

#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_IDENTIFY,1,2);
#endif
}