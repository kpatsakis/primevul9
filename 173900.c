static u8 *sony_report_fixup(struct hid_device *hdev, u8 *rdesc,
		unsigned int *rsize)
{
	struct sony_sc *sc = hid_get_drvdata(hdev);

	if (sc->quirks & (SINO_LITE_CONTROLLER | FUTUREMAX_DANCE_MAT))
		return rdesc;

	/*
	 * Some Sony RF receivers wrongly declare the mouse pointer as a
	 * a constant non-data variable.
	 */
	if ((sc->quirks & VAIO_RDESC_CONSTANT) && *rsize >= 56 &&
	    /* usage page: generic desktop controls */
	    /* rdesc[0] == 0x05 && rdesc[1] == 0x01 && */
	    /* usage: mouse */
	    rdesc[2] == 0x09 && rdesc[3] == 0x02 &&
	    /* input (usage page for x,y axes): constant, variable, relative */
	    rdesc[54] == 0x81 && rdesc[55] == 0x07) {
		hid_info(hdev, "Fixing up Sony RF Receiver report descriptor\n");
		/* input: data, variable, relative */
		rdesc[55] = 0x06;
	}

	if (sc->quirks & MOTION_CONTROLLER)
		return motion_fixup(hdev, rdesc, rsize);

	if (sc->quirks & PS3REMOTE)
		return ps3remote_fixup(hdev, rdesc, rsize);

	return rdesc;
}