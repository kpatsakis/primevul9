void LibRaw::kodak_thumb_loader()
{
    // some kodak cameras
    ushort s_height = S.height, s_width = S.width,s_iwidth = S.iwidth,s_iheight=S.iheight;
    int s_colors = P1.colors;
    unsigned s_filters = P1.filters;
    ushort (*s_image)[4] = imgdata.image;

    
    S.height = T.theight;
    S.width  = T.twidth;
    P1.filters = 0;

    if (thumb_load_raw == &CLASS kodak_ycbcr_load_thumb) 
        {
            S.height += S.height & 1;
            S.width  += S.width  & 1;
        }
    
    imgdata.image = (ushort (*)[4]) calloc (S.iheight*S.iwidth, sizeof (*imgdata.image));
    merror (imgdata.image, "LibRaw::kodak_thumb_loader()");

    ID.input->seek(ID.toffset, SEEK_SET);
    // read kodak thumbnail into T.image[]
    (this->*thumb_load_raw)();

    // copy-n-paste from image pipe
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define LIM(x,min,max) MAX(min,MIN(x,max))
#define CLIP(x) LIM(x,0,65535)
#define SWAP(a,b) { a ^= b; a ^= (b ^= a); }

    // from scale_colors
    {
        double   dmax;
        float scale_mul[4];
        int c,val;
        for (dmax=DBL_MAX, c=0; c < 3; c++) 
                if (dmax > C.pre_mul[c])
                    dmax = C.pre_mul[c];

        for( c=0; c< 3; c++)
                scale_mul[c] = (C.pre_mul[c] / dmax) * 65535.0 / C.maximum;
        scale_mul[3] = scale_mul[1];

        size_t size = S.height * S.width;
        for (unsigned i=0; i < size*4 ; i++) 
            {
                val = imgdata.image[0][i];
                if(!val) continue;
                val *= scale_mul[i & 3];
                imgdata.image[0][i] = CLIP(val);
            }
    }

    // from convert_to_rgb
    ushort *img;
    int row,col;
    
    int  (*t_hist)[LIBRAW_HISTOGRAM_SIZE] =  (int (*)[LIBRAW_HISTOGRAM_SIZE]) calloc(sizeof(*t_hist),4);
    merror (t_hist, "LibRaw::kodak_thumb_loader()");
    
    float out[3], 
        out_cam[3][4] = 
        {
            {2.81761312, -1.98369181, 0.166078627, 0}, 
            {-0.111855984, 1.73688626, -0.625030339, 0}, 
            {-0.0379119813, -0.891268849, 1.92918086, 0}
        };

    for (img=imgdata.image[0], row=0; row < S.height; row++)
        for (col=0; col < S.width; col++, img+=4)
            {
                out[0] = out[1] = out[2] = 0;
                int c;
                for(c=0;c<3;c++) 
                    {
                        out[0] += out_cam[0][c] * img[c];
                        out[1] += out_cam[1][c] * img[c];
                        out[2] += out_cam[2][c] * img[c];
                    }
                for(c=0; c<3; c++)
                    img[c] = CLIP((int) out[c]);
                for(c=0; c<P1.colors;c++)
                    t_hist[c][img[c] >> 3]++;
                    
            }

    // from gamma_lut
    int  (*save_hist)[LIBRAW_HISTOGRAM_SIZE] = libraw_internal_data.output_data.histogram;
    libraw_internal_data.output_data.histogram = t_hist;

    // make curve output curve!
    ushort (*t_curve) = (ushort*) calloc(sizeof(C.curve),1);
    merror (t_curve, "LibRaw::kodak_thumb_loader()");
    memmove(t_curve,C.curve,sizeof(C.curve));
    memset(C.curve,0,sizeof(C.curve));
        {
            int perc, val, total, t_white=0x2000,c;

            perc = S.width * S.height * 0.01;		/* 99th percentile white level */
            if (IO.fuji_width) perc /= 2;
            if (!((O.highlight & ~2) || O.no_auto_bright))
                for (t_white=c=0; c < P1.colors; c++) {
                    for (val=0x2000, total=0; --val > 32; )
                        if ((total += libraw_internal_data.output_data.histogram[c][val]) > perc) break;
                    if (t_white < val) t_white = val;
                }
            gamma_curve (O.gamm[0], O.gamm[1], 2, (t_white << 3)/O.bright);
        }
    
    libraw_internal_data.output_data.histogram = save_hist;
    free(t_hist);
    
    // from write_ppm_tiff - copy pixels into bitmap
    
    S.iheight = S.height;
    S.iwidth  = S.width;
    if (S.flip & 4) SWAP(S.height,S.width);

    if(T.thumb) free(T.thumb);
    T.thumb = (char*) calloc (S.width * S.height, P1.colors);
    merror (T.thumb, "LibRaw::kodak_thumb_loader()");
    T.tlength = S.width * S.height * P1.colors;

    // from write_tiff_ppm
    {
        int soff  = flip_index (0, 0);
        int cstep = flip_index (0, 1) - soff;
        int rstep = flip_index (1, 0) - flip_index (0, S.width);
        
        for (int row=0; row < S.height; row++, soff += rstep) 
            {
                char *ppm = T.thumb + row*S.width*P1.colors;
                for (int col=0; col < S.width; col++, soff += cstep)
                    for(int c = 0; c < P1.colors; c++)
                        ppm [col*P1.colors+c] = imgdata.color.curve[imgdata.image[soff][c]]>>8;
            }
    }

    memmove(C.curve,t_curve,sizeof(C.curve));
    free(t_curve);

    // restore variables
    free(imgdata.image);
    imgdata.image  = s_image;
    
    T.twidth = S.width;
    S.width = s_width;

    S.iwidth = s_iwidth;
    S.iheight = s_iheight;

    T.theight = S.height;
    S.height = s_height;

    T.tcolors = P1.colors;
    P1.colors = s_colors;

    P1.filters = s_filters;
}