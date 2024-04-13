std::unique_ptr<JBIG2Bitmap> JBIG2Stream::readGenericRefinementRegion(int w, int h, int templ, bool tpgrOn, JBIG2Bitmap *refBitmap, int refDX, int refDY, int *atx, int *aty)
{
    bool ltp;
    unsigned int ltpCX, cx, cx0, cx2, cx3, cx4, tpgrCX0, tpgrCX1, tpgrCX2;
    JBIG2BitmapPtr cxPtr0 = { nullptr, 0, 0 };
    JBIG2BitmapPtr cxPtr1 = { nullptr, 0, 0 };
    JBIG2BitmapPtr cxPtr2 = { nullptr, 0, 0 };
    JBIG2BitmapPtr cxPtr3 = { nullptr, 0, 0 };
    JBIG2BitmapPtr cxPtr4 = { nullptr, 0, 0 };
    JBIG2BitmapPtr cxPtr5 = { nullptr, 0, 0 };
    JBIG2BitmapPtr cxPtr6 = { nullptr, 0, 0 };
    JBIG2BitmapPtr tpgrCXPtr0 = { nullptr, 0, 0 };
    JBIG2BitmapPtr tpgrCXPtr1 = { nullptr, 0, 0 };
    JBIG2BitmapPtr tpgrCXPtr2 = { nullptr, 0, 0 };
    int x, y, pix;

    if (!refBitmap) {
        return nullptr;
    }

    auto bitmap = std::make_unique<JBIG2Bitmap>(0, w, h);
    if (!bitmap->isOk()) {
        return nullptr;
    }
    bitmap->clearToZero();

    // set up the typical row context
    if (templ) {
        ltpCX = 0x008;
    } else {
        ltpCX = 0x0010;
    }

    ltp = false;
    for (y = 0; y < h; ++y) {

        if (templ) {

            // set up the context
            bitmap->getPixelPtr(0, y - 1, &cxPtr0);
            cx0 = bitmap->nextPixel(&cxPtr0);
            bitmap->getPixelPtr(-1, y, &cxPtr1);
            refBitmap->getPixelPtr(-refDX, y - 1 - refDY, &cxPtr2);
            refBitmap->getPixelPtr(-1 - refDX, y - refDY, &cxPtr3);
            cx3 = refBitmap->nextPixel(&cxPtr3);
            cx3 = (cx3 << 1) | refBitmap->nextPixel(&cxPtr3);
            refBitmap->getPixelPtr(-refDX, y + 1 - refDY, &cxPtr4);
            cx4 = refBitmap->nextPixel(&cxPtr4);

            // set up the typical prediction context
            tpgrCX0 = tpgrCX1 = tpgrCX2 = 0; // make gcc happy
            if (tpgrOn) {
                refBitmap->getPixelPtr(-1 - refDX, y - 1 - refDY, &tpgrCXPtr0);
                tpgrCX0 = refBitmap->nextPixel(&tpgrCXPtr0);
                tpgrCX0 = (tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0);
                tpgrCX0 = (tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0);
                refBitmap->getPixelPtr(-1 - refDX, y - refDY, &tpgrCXPtr1);
                tpgrCX1 = refBitmap->nextPixel(&tpgrCXPtr1);
                tpgrCX1 = (tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1);
                tpgrCX1 = (tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1);
                refBitmap->getPixelPtr(-1 - refDX, y + 1 - refDY, &tpgrCXPtr2);
                tpgrCX2 = refBitmap->nextPixel(&tpgrCXPtr2);
                tpgrCX2 = (tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2);
                tpgrCX2 = (tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2);
            } else {
                tpgrCXPtr0.p = tpgrCXPtr1.p = tpgrCXPtr2.p = nullptr; // make gcc happy
                tpgrCXPtr0.shift = tpgrCXPtr1.shift = tpgrCXPtr2.shift = 0;
                tpgrCXPtr0.x = tpgrCXPtr1.x = tpgrCXPtr2.x = 0;
            }

            for (x = 0; x < w; ++x) {

                // update the context
                cx0 = ((cx0 << 1) | bitmap->nextPixel(&cxPtr0)) & 7;
                cx3 = ((cx3 << 1) | refBitmap->nextPixel(&cxPtr3)) & 7;
                cx4 = ((cx4 << 1) | refBitmap->nextPixel(&cxPtr4)) & 3;

                if (tpgrOn) {
                    // update the typical predictor context
                    tpgrCX0 = ((tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0)) & 7;
                    tpgrCX1 = ((tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1)) & 7;
                    tpgrCX2 = ((tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2)) & 7;

                    // check for a "typical" pixel
                    if (arithDecoder->decodeBit(ltpCX, refinementRegionStats)) {
                        ltp = !ltp;
                    }
                    if (tpgrCX0 == 0 && tpgrCX1 == 0 && tpgrCX2 == 0) {
                        bitmap->clearPixel(x, y);
                        continue;
                    } else if (tpgrCX0 == 7 && tpgrCX1 == 7 && tpgrCX2 == 7) {
                        bitmap->setPixel(x, y);
                        continue;
                    }
                }

                // build the context
                cx = (cx0 << 7) | (bitmap->nextPixel(&cxPtr1) << 6) | (refBitmap->nextPixel(&cxPtr2) << 5) | (cx3 << 2) | cx4;

                // decode the pixel
                if ((pix = arithDecoder->decodeBit(cx, refinementRegionStats))) {
                    bitmap->setPixel(x, y);
                }
            }

        } else {

            // set up the context
            bitmap->getPixelPtr(0, y - 1, &cxPtr0);
            cx0 = bitmap->nextPixel(&cxPtr0);
            bitmap->getPixelPtr(-1, y, &cxPtr1);
            refBitmap->getPixelPtr(-refDX, y - 1 - refDY, &cxPtr2);
            cx2 = refBitmap->nextPixel(&cxPtr2);
            refBitmap->getPixelPtr(-1 - refDX, y - refDY, &cxPtr3);
            cx3 = refBitmap->nextPixel(&cxPtr3);
            cx3 = (cx3 << 1) | refBitmap->nextPixel(&cxPtr3);
            refBitmap->getPixelPtr(-1 - refDX, y + 1 - refDY, &cxPtr4);
            cx4 = refBitmap->nextPixel(&cxPtr4);
            cx4 = (cx4 << 1) | refBitmap->nextPixel(&cxPtr4);
            bitmap->getPixelPtr(atx[0], y + aty[0], &cxPtr5);
            refBitmap->getPixelPtr(atx[1] - refDX, y + aty[1] - refDY, &cxPtr6);

            // set up the typical prediction context
            tpgrCX0 = tpgrCX1 = tpgrCX2 = 0; // make gcc happy
            if (tpgrOn) {
                refBitmap->getPixelPtr(-1 - refDX, y - 1 - refDY, &tpgrCXPtr0);
                tpgrCX0 = refBitmap->nextPixel(&tpgrCXPtr0);
                tpgrCX0 = (tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0);
                tpgrCX0 = (tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0);
                refBitmap->getPixelPtr(-1 - refDX, y - refDY, &tpgrCXPtr1);
                tpgrCX1 = refBitmap->nextPixel(&tpgrCXPtr1);
                tpgrCX1 = (tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1);
                tpgrCX1 = (tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1);
                refBitmap->getPixelPtr(-1 - refDX, y + 1 - refDY, &tpgrCXPtr2);
                tpgrCX2 = refBitmap->nextPixel(&tpgrCXPtr2);
                tpgrCX2 = (tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2);
                tpgrCX2 = (tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2);
            } else {
                tpgrCXPtr0.p = tpgrCXPtr1.p = tpgrCXPtr2.p = nullptr; // make gcc happy
                tpgrCXPtr0.shift = tpgrCXPtr1.shift = tpgrCXPtr2.shift = 0;
                tpgrCXPtr0.x = tpgrCXPtr1.x = tpgrCXPtr2.x = 0;
            }

            for (x = 0; x < w; ++x) {

                // update the context
                cx0 = ((cx0 << 1) | bitmap->nextPixel(&cxPtr0)) & 3;
                cx2 = ((cx2 << 1) | refBitmap->nextPixel(&cxPtr2)) & 3;
                cx3 = ((cx3 << 1) | refBitmap->nextPixel(&cxPtr3)) & 7;
                cx4 = ((cx4 << 1) | refBitmap->nextPixel(&cxPtr4)) & 7;

                if (tpgrOn) {
                    // update the typical predictor context
                    tpgrCX0 = ((tpgrCX0 << 1) | refBitmap->nextPixel(&tpgrCXPtr0)) & 7;
                    tpgrCX1 = ((tpgrCX1 << 1) | refBitmap->nextPixel(&tpgrCXPtr1)) & 7;
                    tpgrCX2 = ((tpgrCX2 << 1) | refBitmap->nextPixel(&tpgrCXPtr2)) & 7;

                    // check for a "typical" pixel
                    if (arithDecoder->decodeBit(ltpCX, refinementRegionStats)) {
                        ltp = !ltp;
                    }
                    if (tpgrCX0 == 0 && tpgrCX1 == 0 && tpgrCX2 == 0) {
                        bitmap->clearPixel(x, y);
                        continue;
                    } else if (tpgrCX0 == 7 && tpgrCX1 == 7 && tpgrCX2 == 7) {
                        bitmap->setPixel(x, y);
                        continue;
                    }
                }

                // build the context
                cx = (cx0 << 11) | (bitmap->nextPixel(&cxPtr1) << 10) | (cx2 << 8) | (cx3 << 5) | (cx4 << 2) | (bitmap->nextPixel(&cxPtr5) << 1) | refBitmap->nextPixel(&cxPtr6);

                // decode the pixel
                if ((pix = arithDecoder->decodeBit(cx, refinementRegionStats))) {
                    bitmap->setPixel(x, y);
                }
            }
        }
    }

    return bitmap;
}