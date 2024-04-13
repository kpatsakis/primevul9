ptp_pack_EK_text(PTPParams *params, PTPEKTextParams *text, unsigned char **data) {
	int i, len = 0;
	uint8_t	retlen;
	unsigned char *curdata;

	len =	2*(strlen(text->title)+1)+1+
		2*(strlen(text->line[0])+1)+1+
		2*(strlen(text->line[1])+1)+1+
		2*(strlen(text->line[2])+1)+1+
		2*(strlen(text->line[3])+1)+1+
		2*(strlen(text->line[4])+1)+1+
		4*2+2*4+2+4+2+5*4*2;
	*data = malloc(len);
	if (!*data) return 0;

	curdata = *data;
	htod16a(curdata,100);curdata+=2;
	htod16a(curdata,1);curdata+=2;
	htod16a(curdata,0);curdata+=2;
	htod16a(curdata,1000);curdata+=2;

	htod32a(curdata,0);curdata+=4;
	htod32a(curdata,0);curdata+=4;

	htod16a(curdata,6);curdata+=2;
	htod32a(curdata,0);curdata+=4;

	ptp_pack_string(params, text->title, curdata, 0, &retlen); curdata+=2*retlen+1;htod16a(curdata,0);curdata+=2;
	htod16a(curdata,0x10);curdata+=2;
	
	for (i=0;i<5;i++) {
		ptp_pack_string(params, text->line[i], curdata, 0, &retlen); curdata+=2*retlen+1;htod16a(curdata,0);curdata+=2;
		htod16a(curdata,0x10);curdata+=2;
		htod16a(curdata,0x01);curdata+=2;
		htod16a(curdata,0x02);curdata+=2;
		htod16a(curdata,0x06);curdata+=2;
	}
	return len;
}