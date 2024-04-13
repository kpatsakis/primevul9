static double *GenerateCoefficients(const Image *image,
  DistortImageMethod *method,const size_t number_arguments,
  const double *arguments,size_t number_values,ExceptionInfo *exception)
{
  double
    *coeff;

  size_t
    i;

  size_t
    number_coeff, /* number of coefficients to return (array size) */
    cp_size,      /* number floating point numbers per control point */
    cp_x,cp_y,    /* the x,y indexes for control point */
    cp_values;    /* index of values for this control point */
    /* number_values   Number of values given per control point */

  if ( number_values == 0 ) {
    /* Image distortion using control points (or other distortion)
       That is generate a mapping so that   x,y->u,v   given  u,v,x,y
    */
    number_values = 2;   /* special case: two values of u,v */
    cp_values = 0;       /* the values i,j are BEFORE the destination CP x,y */
    cp_x = 2;            /* location of x,y in input control values */
    cp_y = 3;
    /* NOTE: cp_values, also used for later 'reverse map distort' tests */
  }
  else {
    cp_x = 0;            /* location of x,y in input control values */
    cp_y = 1;
    cp_values = 2;       /* and the other values are after x,y */
    /* Typically in this case the values are R,G,B color values */
  }
  cp_size = number_values+2; /* each CP defintion involves this many numbers */

  /* If not enough control point pairs are found for specific distortions
     fall back to Affine distortion (allowing 0 to 3 point pairs)
  */
  if ( number_arguments < 4*cp_size &&
       (  *method == BilinearForwardDistortion
       || *method == BilinearReverseDistortion
       || *method == PerspectiveDistortion
       ) )
    *method = AffineDistortion;

  number_coeff=0;
  switch (*method) {
    case AffineDistortion:
    /* also BarycentricColorInterpolate: */
      number_coeff=3*number_values;
      break;
    case PolynomialDistortion:
      /* number of coefficents depend on the given polynomal 'order' */
      i = poly_number_terms(arguments[0]);
      number_coeff = 2 + i*number_values;
      if ( i == 0 ) {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
                   "InvalidArgument","%s : '%s'","Polynomial",
                   "Invalid order, should be interger 1 to 5, or 1.5");
        return((double *) NULL);
      }
      if ( number_arguments < 1+i*cp_size ) {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
               "InvalidArgument", "%s : 'require at least %.20g CPs'",
               "Polynomial", (double) i);
        return((double *) NULL);
      }
      break;
    case BilinearReverseDistortion:
      number_coeff=4*number_values;
      break;
    /*
      The rest are constants as they are only used for image distorts
    */
    case BilinearForwardDistortion:
      number_coeff=10; /* 2*4 coeff plus 2 constants */
      cp_x = 0;        /* Reverse src/dest coords for forward mapping */
      cp_y = 1;
      cp_values = 2;
      break;
#if 0
    case QuadraterialDistortion:
      number_coeff=19; /* BilinearForward + BilinearReverse */
#endif
      break;
    case ShepardsDistortion:
      number_coeff=1;  /* The power factor to use */
      break;
    case ArcDistortion:
      number_coeff=5;
      break;
    case ScaleRotateTranslateDistortion:
    case AffineProjectionDistortion:
    case Plane2CylinderDistortion:
    case Cylinder2PlaneDistortion:
      number_coeff=6;
      break;
    case PolarDistortion:
    case DePolarDistortion:
      number_coeff=8;
      break;
    case PerspectiveDistortion:
    case PerspectiveProjectionDistortion:
      number_coeff=9;
      break;
    case BarrelDistortion:
    case BarrelInverseDistortion:
      number_coeff=10;
      break;
    default:
      perror("unknown method given"); /* just fail assertion */
  }

  /* allocate the array of coefficients needed */
  coeff = (double *) AcquireQuantumMemory(number_coeff,sizeof(*coeff));
  if (coeff == (double *) NULL) {
    (void) ThrowMagickException(exception,GetMagickModule(),
                  ResourceLimitError,"MemoryAllocationFailed",
                  "%s", "GenerateCoefficients");
    return((double *) NULL);
  }

  /* zero out coefficients array */
  for (i=0; i < number_coeff; i++)
    coeff[i] = 0.0;

  switch (*method)
  {
    case AffineDistortion:
    {
      /* Affine Distortion
           v =  c0*x + c1*y + c2
         for each 'value' given

         Input Arguments are sets of control points...
         For Distort Images    u,v, x,y  ...
         For Sparse Gradients  x,y, r,g,b  ...
      */
      if ( number_arguments%cp_size != 0 ||
           number_arguments < cp_size ) {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
               "InvalidArgument", "%s : 'require at least %.20g CPs'",
               "Affine", 1.0);
        coeff=(double *) RelinquishMagickMemory(coeff);
        return((double *) NULL);
      }
      /* handle special cases of not enough arguments */
      if ( number_arguments == cp_size ) {
        /* Only 1 CP Set Given */
        if ( cp_values == 0 ) {
          /* image distortion - translate the image */
          coeff[0] = 1.0;
          coeff[2] = arguments[0] - arguments[2];
          coeff[4] = 1.0;
          coeff[5] = arguments[1] - arguments[3];
        }
        else {
          /* sparse gradient - use the values directly */
          for (i=0; i<number_values; i++)
            coeff[i*3+2] = arguments[cp_values+i];
        }
      }
      else {
        /* 2 or more points (usally 3) given.
           Solve a least squares simultaneous equation for coefficients.
        */
        double
          **matrix,
          **vectors,
          terms[3];

        MagickBooleanType
          status;

        /* create matrix, and a fake vectors matrix */
        matrix = AcquireMagickMatrix(3UL,3UL);
        vectors = (double **) AcquireQuantumMemory(number_values,sizeof(*vectors));
        if (matrix == (double **) NULL || vectors == (double **) NULL)
        {
          matrix  = RelinquishMagickMatrix(matrix, 3UL);
          vectors = (double **) RelinquishMagickMemory(vectors);
          coeff   = (double *) RelinquishMagickMemory(coeff);
          (void) ThrowMagickException(exception,GetMagickModule(),
                  ResourceLimitError,"MemoryAllocationFailed",
                  "%s", "DistortCoefficients");
          return((double *) NULL);
        }
        /* fake a number_values x3 vectors matrix from coefficients array */
        for (i=0; i < number_values; i++)
          vectors[i] = &(coeff[i*3]);
        /* Add given control point pairs for least squares solving */
        for (i=0; i < number_arguments; i+=cp_size) {
          terms[0] = arguments[i+cp_x];  /* x */
          terms[1] = arguments[i+cp_y];  /* y */
          terms[2] = 1;                  /* 1 */
          LeastSquaresAddTerms(matrix,vectors,terms,
                   &(arguments[i+cp_values]),3UL,number_values);
        }
        if ( number_arguments == 2*cp_size ) {
          /* Only two pairs were given, but we need 3 to solve the affine.
             Fake extra coordinates by rotating p1 around p0 by 90 degrees.
               x2 = x0 - (y1-y0)   y2 = y0 + (x1-x0)
           */
          terms[0] = arguments[cp_x]
                   - ( arguments[cp_size+cp_y] - arguments[cp_y] ); /* x2 */
          terms[1] = arguments[cp_y] +
                   + ( arguments[cp_size+cp_x] - arguments[cp_x] ); /* y2 */
          terms[2] = 1;                                             /* 1 */
          if ( cp_values == 0 ) {
            /* Image Distortion - rotate the u,v coordients too */
            double
              uv2[2];
            uv2[0] = arguments[0] - arguments[5] + arguments[1];   /* u2 */
            uv2[1] = arguments[1] + arguments[4] - arguments[0];   /* v2 */
            LeastSquaresAddTerms(matrix,vectors,terms,uv2,3UL,2UL);
          }
          else {
            /* Sparse Gradient - use values of p0 for linear gradient */
            LeastSquaresAddTerms(matrix,vectors,terms,
                  &(arguments[cp_values]),3UL,number_values);
          }
        }
        /* Solve for LeastSquares Coefficients */
        status=GaussJordanElimination(matrix,vectors,3UL,number_values);
        matrix = RelinquishMagickMatrix(matrix, 3UL);
        vectors = (double **) RelinquishMagickMemory(vectors);
        if ( status == MagickFalse ) {
          coeff = (double *) RelinquishMagickMemory(coeff);
          (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
              "InvalidArgument","%s : 'Unsolvable Matrix'",
              CommandOptionToMnemonic(MagickDistortOptions, *method) );
          return((double *) NULL);
        }
      }
      return(coeff);
    }
    case AffineProjectionDistortion:
    {
      /*
        Arguments: Affine Matrix (forward mapping)
        Arguments  sx, rx, ry, sy, tx, ty
        Where      u = sx*x + ry*y + tx
                   v = rx*x + sy*y + ty

        Returns coefficients (in there inverse form) ordered as...
             sx ry tx  rx sy ty

        AffineProjection Distortion Notes...
           + Will only work with a 2 number_values for Image Distortion
           + Can not be used for generating a sparse gradient (interpolation)
      */
      double inverse[8];
      if (number_arguments != 6) {
        coeff = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
              "InvalidArgument","%s : 'Needs 6 coeff values'",
              CommandOptionToMnemonic(MagickDistortOptions, *method) );
        return((double *) NULL);
      }
      /* FUTURE: trap test for sx*sy-rx*ry == 0 (determinant = 0, no inverse) */
      for(i=0; i<6UL; i++ )
        inverse[i] = arguments[i];
      AffineArgsToCoefficients(inverse); /* map into coefficents */
      InvertAffineCoefficients(inverse, coeff); /* invert */
      *method = AffineDistortion;

      return(coeff);
    }
    case ScaleRotateTranslateDistortion:
    {
      /* Scale, Rotate and Translate Distortion
         An alternative Affine Distortion
         Argument options, by number of arguments given:
           7: x,y, sx,sy, a, nx,ny
           6: x,y,   s,   a, nx,ny
           5: x,y, sx,sy, a
           4: x,y,   s,   a
           3: x,y,        a
           2:        s,   a
           1:             a
         Where actions are (in order of application)
            x,y     'center' of transforms     (default = image center)
            sx,sy   scale image by this amount (default = 1)
            a       angle of rotation          (argument required)
            nx,ny   move 'center' here         (default = x,y or no movement)
         And convert to affine mapping coefficients

         ScaleRotateTranslate Distortion Notes...
           + Does not use a set of CPs in any normal way
           + Will only work with a 2 number_valuesal Image Distortion
           + Cannot be used for generating a sparse gradient (interpolation)
      */
      double
        cosine, sine,
        x,y,sx,sy,a,nx,ny;

      /* set default center, and default scale */
      x = nx = (double)(image->columns)/2.0 + (double)image->page.x;
      y = ny = (double)(image->rows)/2.0    + (double)image->page.y;
      sx = sy = 1.0;
      switch ( number_arguments ) {
      case 0:
        coeff = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
              "InvalidArgument","%s : 'Needs at least 1 argument'",
              CommandOptionToMnemonic(MagickDistortOptions, *method) );
        return((double *) NULL);
      case 1:
        a = arguments[0];
        break;
      case 2:
        sx = sy = arguments[0];
        a = arguments[1];
        break;
      default:
        x = nx = arguments[0];
        y = ny = arguments[1];
        switch ( number_arguments ) {
        case 3:
          a = arguments[2];
          break;
        case 4:
          sx = sy = arguments[2];
          a = arguments[3];
          break;
        case 5:
          sx = arguments[2];
          sy = arguments[3];
          a = arguments[4];
          break;
        case 6:
          sx = sy = arguments[2];
          a = arguments[3];
          nx = arguments[4];
          ny = arguments[5];
          break;
        case 7:
          sx = arguments[2];
          sy = arguments[3];
          a = arguments[4];
          nx = arguments[5];
          ny = arguments[6];
          break;
        default:
          coeff = (double *) RelinquishMagickMemory(coeff);
          (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
              "InvalidArgument","%s : 'Too Many Arguments (7 or less)'",
              CommandOptionToMnemonic(MagickDistortOptions, *method) );
          return((double *) NULL);
        }
        break;
      }
      /* Trap if sx or sy == 0 -- image is scaled out of existance! */
      if ( fabs(sx) < MagickEpsilon || fabs(sy) < MagickEpsilon ) {
        coeff = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
              "InvalidArgument","%s : 'Zero Scale Given'",
              CommandOptionToMnemonic(MagickDistortOptions, *method) );
        return((double *) NULL);
      }
      /* Save the given arguments as an affine distortion */
      a=DegreesToRadians(a); cosine=cos(a); sine=sin(a);

      *method = AffineDistortion;
      coeff[0]=cosine/sx;
      coeff[1]=sine/sx;
      coeff[2]=x-nx*coeff[0]-ny*coeff[1];
      coeff[3]=(-sine)/sy;
      coeff[4]=cosine/sy;
      coeff[5]=y-nx*coeff[3]-ny*coeff[4];
      return(coeff);
    }
    case PerspectiveDistortion:
    { /*
         Perspective Distortion (a ratio of affine distortions)

                p(x,y)    c0*x + c1*y + c2
            u = ------ = ------------------
                r(x,y)    c6*x + c7*y + 1

                q(x,y)    c3*x + c4*y + c5
            v = ------ = ------------------
                r(x,y)    c6*x + c7*y + 1

           c8 = Sign of 'r', or the denominator affine, for the actual image.
                This determines what part of the distorted image is 'ground'
                side of the horizon, the other part is 'sky' or invalid.
                Valid values are  +1.0  or  -1.0  only.

         Input Arguments are sets of control points...
         For Distort Images    u,v, x,y  ...
         For Sparse Gradients  x,y, r,g,b  ...

         Perspective Distortion Notes...
           + Can be thought of as ratio of  3 affine transformations
           + Not separatable: r() or c6 and c7 are used by both equations
           + All 8 coefficients must be determined simultaniously
           + Will only work with a 2 number_valuesal Image Distortion
           + Can not be used for generating a sparse gradient (interpolation)
           + It is not linear, but is simple to generate an inverse
           + All lines within an image remain lines.
           + but distances between points may vary.
      */
      double
        **matrix,
        *vectors[1],
        terms[8];

      size_t
        cp_u = cp_values,
        cp_v = cp_values+1;

      MagickBooleanType
        status;

      if ( number_arguments%cp_size != 0 ||
           number_arguments < cp_size*4 ) {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
              "InvalidArgument", "%s : 'require at least %.20g CPs'",
              CommandOptionToMnemonic(MagickDistortOptions, *method), 4.0);
        coeff=(double *) RelinquishMagickMemory(coeff);
        return((double *) NULL);
      }
      /* fake 1x8 vectors matrix directly using the coefficients array */
      vectors[0] = &(coeff[0]);
      /* 8x8 least-squares matrix (zeroed) */
      matrix = AcquireMagickMatrix(8UL,8UL);
      if (matrix == (double **) NULL) {
        coeff=(double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),
                  ResourceLimitError,"MemoryAllocationFailed",
                  "%s", "DistortCoefficients");
        return((double *) NULL);
      }
      /* Add control points for least squares solving */
      for (i=0; i < number_arguments; i+=4) {
        terms[0]=arguments[i+cp_x];            /*   c0*x   */
        terms[1]=arguments[i+cp_y];            /*   c1*y   */
        terms[2]=1.0;                          /*   c2*1   */
        terms[3]=0.0;
        terms[4]=0.0;
        terms[5]=0.0;
        terms[6]=-terms[0]*arguments[i+cp_u];  /* 1/(c6*x) */
        terms[7]=-terms[1]*arguments[i+cp_u];  /* 1/(c7*y) */
        LeastSquaresAddTerms(matrix,vectors,terms,&(arguments[i+cp_u]),
            8UL,1UL);

        terms[0]=0.0;
        terms[1]=0.0;
        terms[2]=0.0;
        terms[3]=arguments[i+cp_x];           /*   c3*x   */
        terms[4]=arguments[i+cp_y];           /*   c4*y   */
        terms[5]=1.0;                         /*   c5*1   */
        terms[6]=-terms[3]*arguments[i+cp_v]; /* 1/(c6*x) */
        terms[7]=-terms[4]*arguments[i+cp_v]; /* 1/(c7*y) */
        LeastSquaresAddTerms(matrix,vectors,terms,&(arguments[i+cp_v]),
            8UL,1UL);
      }
      /* Solve for LeastSquares Coefficients */
      status=GaussJordanElimination(matrix,vectors,8UL,1UL);
      matrix = RelinquishMagickMatrix(matrix, 8UL);
      if ( status == MagickFalse ) {
        coeff = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "InvalidArgument","%s : 'Unsolvable Matrix'",
            CommandOptionToMnemonic(MagickDistortOptions, *method) );
        return((double *) NULL);
      }
      /*
        Calculate 9'th coefficient! The ground-sky determination.
        What is sign of the 'ground' in r() denominator affine function?
        Just use any valid image coordinate (first control point) in
        destination for determination of what part of view is 'ground'.
      */
      coeff[8] = coeff[6]*arguments[cp_x]
                      + coeff[7]*arguments[cp_y] + 1.0;
      coeff[8] = (coeff[8] < 0.0) ? -1.0 : +1.0;

      return(coeff);
    }
    case PerspectiveProjectionDistortion:
    {
      /*
        Arguments: Perspective Coefficents (forward mapping)
      */
      if (number_arguments != 8) {
        coeff = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
              "InvalidArgument", "%s : 'Needs 8 coefficient values'",
              CommandOptionToMnemonic(MagickDistortOptions, *method));
        return((double *) NULL);
      }
      /* FUTURE: trap test  c0*c4-c3*c1 == 0  (determinate = 0, no inverse) */
      InvertPerspectiveCoefficients(arguments, coeff);
      /*
        Calculate 9'th coefficient! The ground-sky determination.
        What is sign of the 'ground' in r() denominator affine function?
        Just use any valid image cocodinate in destination for determination.
        For a forward mapped perspective the images 0,0 coord will map to
        c2,c5 in the distorted image, so set the sign of denominator of that.
      */
      coeff[8] = coeff[6]*arguments[2]
                           + coeff[7]*arguments[5] + 1.0;
      coeff[8] = (coeff[8] < 0.0) ? -1.0 : +1.0;
      *method = PerspectiveDistortion;

      return(coeff);
    }
    case BilinearForwardDistortion:
    case BilinearReverseDistortion:
    {
      /* Bilinear Distortion (Forward mapping)
            v = c0*x + c1*y + c2*x*y + c3;
         for each 'value' given

         This is actually a simple polynomial Distortion!  The difference
         however is when we need to reverse the above equation to generate a
         BilinearForwardDistortion (see below).

         Input Arguments are sets of control points...
         For Distort Images    u,v, x,y  ...
         For Sparse Gradients  x,y, r,g,b  ...

      */
      double
        **matrix,
        **vectors,
        terms[4];

      MagickBooleanType
        status;

      /* check the number of arguments */
      if ( number_arguments%cp_size != 0 ||
           number_arguments < cp_size*4 ) {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
              "InvalidArgument", "%s : 'require at least %.20g CPs'",
              CommandOptionToMnemonic(MagickDistortOptions, *method), 4.0);
        coeff=(double *) RelinquishMagickMemory(coeff);
        return((double *) NULL);
      }
      /* create matrix, and a fake vectors matrix */
      matrix = AcquireMagickMatrix(4UL,4UL);
      vectors = (double **) AcquireQuantumMemory(number_values,sizeof(*vectors));
      if (matrix == (double **) NULL || vectors == (double **) NULL)
      {
        matrix  = RelinquishMagickMatrix(matrix, 4UL);
        vectors = (double **) RelinquishMagickMemory(vectors);
        coeff   = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),
                ResourceLimitError,"MemoryAllocationFailed",
                "%s", "DistortCoefficients");
        return((double *) NULL);
      }
      /* fake a number_values x4 vectors matrix from coefficients array */
      for (i=0; i < number_values; i++)
        vectors[i] = &(coeff[i*4]);
      /* Add given control point pairs for least squares solving */
      for (i=0; i < number_arguments; i+=cp_size) {
        terms[0] = arguments[i+cp_x];   /*  x  */
        terms[1] = arguments[i+cp_y];   /*  y  */
        terms[2] = terms[0]*terms[1];   /* x*y */
        terms[3] = 1;                   /*  1  */
        LeastSquaresAddTerms(matrix,vectors,terms,
             &(arguments[i+cp_values]),4UL,number_values);
      }
      /* Solve for LeastSquares Coefficients */
      status=GaussJordanElimination(matrix,vectors,4UL,number_values);
      matrix  = RelinquishMagickMatrix(matrix, 4UL);
      vectors = (double **) RelinquishMagickMemory(vectors);
      if ( status == MagickFalse ) {
        coeff = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "InvalidArgument","%s : 'Unsolvable Matrix'",
            CommandOptionToMnemonic(MagickDistortOptions, *method) );
        return((double *) NULL);
      }
      if ( *method == BilinearForwardDistortion ) {
         /* Bilinear Forward Mapped Distortion

         The above least-squares solved for coefficents but in the forward
         direction, due to changes to indexing constants.

            i = c0*x + c1*y + c2*x*y + c3;
            j = c4*x + c5*y + c6*x*y + c7;

         where i,j are in the destination image, NOT the source.

         Reverse Pixel mapping however needs to use reverse of these
         functions.  It required a full page of algbra to work out the
         reversed mapping formula, but resolves down to the following...

            c8 = c0*c5-c1*c4;
            c9 = 2*(c2*c5-c1*c6);   // '2*a' in the quadratic formula

            i = i - c3;   j = j - c7;
            b = c6*i - c2*j + c8;   // So that   a*y^2 + b*y + c == 0
            c = c4*i -  c0*j;       // y = ( -b +- sqrt(bb - 4ac) ) / (2*a)

            r = b*b - c9*(c+c);
            if ( c9 != 0 )
              y = ( -b + sqrt(r) ) / c9;
            else
              y = -c/b;

            x = ( i - c1*y) / ( c1 - c2*y );

         NB: if 'r' is negative there is no solution!
         NB: the sign of the sqrt() should be negative if image becomes
             flipped or flopped, or crosses over itself.
         NB: techniqually coefficient c5 is not needed, anymore,
             but kept for completness.

         See Anthony Thyssen <A.Thyssen@griffith.edu.au>
         or  Fred Weinhaus <fmw@alink.net>  for more details.

         */
         coeff[8] = coeff[0]*coeff[5] - coeff[1]*coeff[4];
         coeff[9] = 2*(coeff[2]*coeff[5] - coeff[1]*coeff[6]);
      }
      return(coeff);
    }
#if 0
    case QuadrilateralDistortion:
    {
      /* Map a Quadrilateral to a unit square using BilinearReverse
         Then map that unit square back to the final Quadrilateral
         using BilinearForward.

         Input Arguments are sets of control points...
         For Distort Images    u,v, x,y  ...
         For Sparse Gradients  x,y, r,g,b  ...

      */
      /* UNDER CONSTRUCTION */
      return(coeff);
    }
#endif

    case PolynomialDistortion:
    {
      /* Polynomial Distortion

         First two coefficents are used to hole global polynomal information
           c0 = Order of the polynimial being created
           c1 = number_of_terms in one polynomial equation

         Rest of the coefficients map to the equations....
            v = c0 + c1*x + c2*y + c3*x*y + c4*x^2 + c5*y^2 + c6*x^3 + ...
         for each 'value' (number_values of them) given.
         As such total coefficients =  2 + number_terms * number_values

         Input Arguments are sets of control points...
         For Distort Images    order  [u,v, x,y] ...
         For Sparse Gradients  order  [x,y, r,g,b] ...

         Polynomial Distortion Notes...
           + UNDER DEVELOPMENT -- Do not expect this to remain as is.
           + Currently polynomial is a reversed mapped distortion.
           + Order 1.5 is fudged to map into a bilinear distortion.
             though it is not the same order as that distortion.
      */
      double
        **matrix,
        **vectors,
        *terms;

      size_t
        nterms;   /* number of polynomial terms per number_values */

      ssize_t
        j;

      MagickBooleanType
        status;

      /* first two coefficients hold polynomial order information */
      coeff[0] = arguments[0];
      coeff[1] = (double) poly_number_terms(arguments[0]);
      nterms = (size_t) coeff[1];

      /* create matrix, a fake vectors matrix, and least sqs terms */
      matrix = AcquireMagickMatrix(nterms,nterms);
      vectors = (double **) AcquireQuantumMemory(number_values,sizeof(*vectors));
      terms = (double *) AcquireQuantumMemory(nterms, sizeof(*terms));
      if (matrix  == (double **) NULL ||
          vectors == (double **) NULL ||
          terms   == (double *) NULL )
      {
        matrix  = RelinquishMagickMatrix(matrix, nterms);
        vectors = (double **) RelinquishMagickMemory(vectors);
        terms   = (double *) RelinquishMagickMemory(terms);
        coeff   = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),
                ResourceLimitError,"MemoryAllocationFailed",
                "%s", "DistortCoefficients");
        return((double *) NULL);
      }
      /* fake a number_values x3 vectors matrix from coefficients array */
      for (i=0; i < number_values; i++)
        vectors[i] = &(coeff[2+i*nterms]);
      /* Add given control point pairs for least squares solving */
      for (i=1; i < number_arguments; i+=cp_size) { /* NB: start = 1 not 0 */
        for (j=0; j < (ssize_t) nterms; j++)
          terms[j] = poly_basis_fn(j,arguments[i+cp_x],arguments[i+cp_y]);
        LeastSquaresAddTerms(matrix,vectors,terms,
             &(arguments[i+cp_values]),nterms,number_values);
      }
      terms = (double *) RelinquishMagickMemory(terms);
      /* Solve for LeastSquares Coefficients */
      status=GaussJordanElimination(matrix,vectors,nterms,number_values);
      matrix  = RelinquishMagickMatrix(matrix, nterms);
      vectors = (double **) RelinquishMagickMemory(vectors);
      if ( status == MagickFalse ) {
        coeff = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "InvalidArgument","%s : 'Unsolvable Matrix'",
            CommandOptionToMnemonic(MagickDistortOptions, *method) );
        return((double *) NULL);
      }
      return(coeff);
    }
    case ArcDistortion:
    {
      /* Arc Distortion
         Args: arc_width  rotate  top_edge_radius  bottom_edge_radius
         All but first argument are optional
            arc_width      The angle over which to arc the image side-to-side
            rotate         Angle to rotate image from vertical center
            top_radius     Set top edge of source image at this radius
            bottom_radius  Set bootom edge to this radius (radial scaling)

         By default, if the radii arguments are nor provided the image radius
         is calculated so the horizontal center-line is fits the given arc
         without scaling.

         The output image size is ALWAYS adjusted to contain the whole image,
         and an offset is given to position image relative to the 0,0 point of
         the origin, allowing users to use relative positioning onto larger
         background (via -flatten).

         The arguments are converted to these coefficients
            c0: angle for center of source image
            c1: angle scale for mapping to source image
            c2: radius for top of source image
            c3: radius scale for mapping source image
            c4: centerline of arc within source image

         Note the coefficients use a center angle, so asymptotic join is
         furthest from both sides of the source image. This also means that
         for arc angles greater than 360 the sides of the image will be
         trimmed equally.

         Arc Distortion Notes...
           + Does not use a set of CPs
           + Will only work with Image Distortion
           + Can not be used for generating a sparse gradient (interpolation)
      */
      if ( number_arguments >= 1 && arguments[0] < MagickEpsilon ) {
        coeff = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "InvalidArgument","%s : 'Arc Angle Too Small'",
            CommandOptionToMnemonic(MagickDistortOptions, *method) );
        return((double *) NULL);
      }
      if ( number_arguments >= 3 && arguments[2] < MagickEpsilon ) {
        coeff = (double *) RelinquishMagickMemory(coeff);
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "InvalidArgument","%s : 'Outer Radius Too Small'",
            CommandOptionToMnemonic(MagickDistortOptions, *method) );
        return((double *) NULL);
      }
      coeff[0] = -MagickPI2;   /* -90, place at top! */
      if ( number_arguments >= 1 )
        coeff[1] = DegreesToRadians(arguments[0]);
      else
        coeff[1] = MagickPI2;   /* zero arguments - center is at top */
      if ( number_arguments >= 2 )
        coeff[0] += DegreesToRadians(arguments[1]);
      coeff[0] /= Magick2PI;  /* normalize radians */
      coeff[0] -= MagickRound(coeff[0]);
      coeff[0] *= Magick2PI;  /* de-normalize back to radians */
      coeff[3] = (double)image->rows-1;
      coeff[2] = (double)image->columns/coeff[1] + coeff[3]/2.0;
      if ( number_arguments >= 3 ) {
        if ( number_arguments >= 4 )
          coeff[3] = arguments[2] - arguments[3];
        else
          coeff[3] *= arguments[2]/coeff[2];
        coeff[2] = arguments[2];
      }
      coeff[4] = ((double)image->columns-1.0)/2.0;

      return(coeff);
    }
    case PolarDistortion:
    case DePolarDistortion:
    {
      /* (De)Polar Distortion   (same set of arguments)
         Args:  Rmax, Rmin,  Xcenter,Ycenter,  Afrom,Ato
         DePolar can also have the extra arguments of Width, Height

         Coefficients 0 to 5 is the sanatized version first 6 input args
         Coefficient 6  is the angle to coord ratio  and visa-versa
         Coefficient 7  is the radius to coord ratio and visa-versa

         WARNING: It is possible for  Radius max<min  and/or  Angle from>to
      */
      if ( number_arguments == 3
          || ( number_arguments > 6 && *method == PolarDistortion )
          || number_arguments > 8 ) {
          (void) ThrowMagickException(exception,GetMagickModule(),
            OptionError,"InvalidArgument", "%s : number of arguments",
            CommandOptionToMnemonic(MagickDistortOptions, *method) );
        coeff=(double *) RelinquishMagickMemory(coeff);
        return((double *) NULL);
      }
      /* Rmax -  if 0 calculate appropriate value */
      if ( number_arguments >= 1 )
        coeff[0] = arguments[0];
      else
        coeff[0] = 0.0;
      /* Rmin  - usally 0 */
      coeff[1] = number_arguments >= 2 ? arguments[1] : 0.0;
      /* Center X,Y */
      if ( number_arguments >= 4 ) {
        coeff[2] = arguments[2];
        coeff[3] = arguments[3];
      }
      else { /* center of actual image */
        coeff[2] = (double)(image->columns)/2.0+image->page.x;
        coeff[3] = (double)(image->rows)/2.0+image->page.y;
      }
      /* Angle from,to - about polar center 0 is downward */
      coeff[4] = -MagickPI;
      if ( number_arguments >= 5 )
        coeff[4] = DegreesToRadians(arguments[4]);
      coeff[5] = coeff[4];
      if ( number_arguments >= 6 )
        coeff[5] = DegreesToRadians(arguments[5]);
      if ( fabs(coeff[4]-coeff[5]) < MagickEpsilon )
        coeff[5] += Magick2PI; /* same angle is a full circle */
      /* if radius 0 or negative,  its a special value... */
      if ( coeff[0] < MagickEpsilon ) {
        /* Use closest edge  if radius == 0 */
        if ( fabs(coeff[0]) < MagickEpsilon ) {
          coeff[0]=MagickMin(fabs(coeff[2]-image->page.x),
                             fabs(coeff[3]-image->page.y));
          coeff[0]=MagickMin(coeff[0],
                       fabs(coeff[2]-image->page.x-image->columns));
          coeff[0]=MagickMin(coeff[0],
                       fabs(coeff[3]-image->page.y-image->rows));
        }
        /* furthest diagonal if radius == -1 */
        if ( fabs(-1.0-coeff[0]) < MagickEpsilon ) {
          double rx,ry;
          rx = coeff[2]-image->page.x;
          ry = coeff[3]-image->page.y;
          coeff[0] = rx*rx+ry*ry;
          ry = coeff[3]-image->page.y-image->rows;
          coeff[0] = MagickMax(coeff[0],rx*rx+ry*ry);
          rx = coeff[2]-image->page.x-image->columns;
          coeff[0] = MagickMax(coeff[0],rx*rx+ry*ry);
          ry = coeff[3]-image->page.y;
          coeff[0] = MagickMax(coeff[0],rx*rx+ry*ry);
          coeff[0] = sqrt(coeff[0]);
        }
      }
      /* IF Rmax <= 0 or Rmin < 0 OR Rmax < Rmin, THEN error */
      if ( coeff[0] < MagickEpsilon || coeff[1] < -MagickEpsilon
           || (coeff[0]-coeff[1]) < MagickEpsilon ) {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "InvalidArgument", "%s : Invalid Radius",
            CommandOptionToMnemonic(MagickDistortOptions, *method) );
        coeff=(double *) RelinquishMagickMemory(coeff);
        return((double *) NULL);
      }
      /* converstion ratios */
      if ( *method == PolarDistortion ) {
        coeff[6]=(double) image->columns/(coeff[5]-coeff[4]);
        coeff[7]=(double) image->rows/(coeff[0]-coeff[1]);
      }
      else { /* *method == DePolarDistortion */
        coeff[6]=(coeff[5]-coeff[4])/image->columns;
        coeff[7]=(coeff[0]-coeff[1])/image->rows;
      }
      return(coeff);
    }
    case Cylinder2PlaneDistortion:
    case Plane2CylinderDistortion:
    {
      /* 3D Cylinder to/from a Tangential Plane

         Projection between a clinder and flat plain from a point on the
         center line of the cylinder.

         The two surfaces coincide in 3D space at the given centers of
         distortion (perpendicular to projection point) on both images.

         Args:  FOV_arc_width
         Coefficents: FOV(radians), Radius, center_x,y, dest_center_x,y

         FOV (Field Of View) the angular field of view of the distortion,
         across the width of the image, in degrees.  The centers are the
         points of least distortion in the input and resulting images.

         These centers are however determined later.

         Coeff 0 is the FOV angle of view of image width in radians
         Coeff 1 is calculated radius of cylinder.
         Coeff 2,3  center of distortion of input image
         Coefficents 4,5 Center of Distortion of dest (determined later)
      */
      if ( arguments[0] < MagickEpsilon || arguments[0] > 160.0 ) {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
            "InvalidArgument", "%s : Invalid FOV Angle",
            CommandOptionToMnemonic(MagickDistortOptions, *method) );
        coeff=(double *) RelinquishMagickMemory(coeff);
        return((double *) NULL);
      }
      coeff[0] = DegreesToRadians(arguments[0]);
      if ( *method == Cylinder2PlaneDistortion )
        /* image is curved around cylinder, so FOV angle (in radians)
         * scales directly to image X coordinate, according to its radius.
         */
        coeff[1] = (double) image->columns/coeff[0];
      else
        /* radius is distance away from an image with this angular FOV */
        coeff[1] = (double) image->columns / ( 2 * tan(coeff[0]/2) );

      coeff[2] = (double)(image->columns)/2.0+image->page.x;
      coeff[3] = (double)(image->rows)/2.0+image->page.y;
      coeff[4] = coeff[2];
      coeff[5] = coeff[3]; /* assuming image size is the same */
      return(coeff);
    }
    case BarrelDistortion:
    case BarrelInverseDistortion:
    {
      /* Barrel Distortion
           Rs=(A*Rd^3 + B*Rd^2 + C*Rd + D)*Rd
         BarrelInv Distortion
           Rs=Rd/(A*Rd^3 + B*Rd^2 + C*Rd + D)

        Where Rd is the normalized radius from corner to middle of image
        Input Arguments are one of the following forms (number of arguments)...
            3:  A,B,C
            4:  A,B,C,D
            5:  A,B,C    X,Y
            6:  A,B,C,D  X,Y
            8:  Ax,Bx,Cx,Dx  Ay,By,Cy,Dy
           10:  Ax,Bx,Cx,Dx  Ay,By,Cy,Dy   X,Y

        Returns 10 coefficent values, which are de-normalized (pixel scale)
          Ax, Bx, Cx, Dx,   Ay, By, Cy, Dy,    Xc, Yc
      */
      /* Radius de-normalization scaling factor */
      double
        rscale = 2.0/MagickMin((double) image->columns,(double) image->rows);

      /* sanity check  number of args must = 3,4,5,6,8,10 or error */
      if ( (number_arguments  < 3) || (number_arguments == 7) ||
           (number_arguments == 9) || (number_arguments > 10) )
        {
          coeff=(double *) RelinquishMagickMemory(coeff);
          (void) ThrowMagickException(exception,GetMagickModule(),
            OptionError,"InvalidArgument", "%s : number of arguments",
            CommandOptionToMnemonic(MagickDistortOptions, *method) );
          return((double *) NULL);
        }
      /* A,B,C,D coefficients */
      coeff[0] = arguments[0];
      coeff[1] = arguments[1];
      coeff[2] = arguments[2];
      if ((number_arguments == 3) || (number_arguments == 5) )
        coeff[3] = 1.0 - coeff[0] - coeff[1] - coeff[2];
      else
        coeff[3] = arguments[3];
      /* de-normalize the coefficients */
      coeff[0] *= pow(rscale,3.0);
      coeff[1] *= rscale*rscale;
      coeff[2] *= rscale;
      /* Y coefficients: as given OR same as X coefficients */
      if ( number_arguments >= 8 ) {
        coeff[4] = arguments[4] * pow(rscale,3.0);
        coeff[5] = arguments[5] * rscale*rscale;
        coeff[6] = arguments[6] * rscale;
        coeff[7] = arguments[7];
      }
      else {
        coeff[4] = coeff[0];
        coeff[5] = coeff[1];
        coeff[6] = coeff[2];
        coeff[7] = coeff[3];
      }
      /* X,Y Center of Distortion (image coodinates) */
      if ( number_arguments == 5 )  {
        coeff[8] = arguments[3];
        coeff[9] = arguments[4];
      }
      else if ( number_arguments == 6 ) {
        coeff[8] = arguments[4];
        coeff[9] = arguments[5];
      }
      else if ( number_arguments == 10 ) {
        coeff[8] = arguments[8];
        coeff[9] = arguments[9];
      }
      else {
        /* center of the image provided (image coodinates) */
        coeff[8] = (double)image->columns/2.0 + image->page.x;
        coeff[9] = (double)image->rows/2.0    + image->page.y;
      }
      return(coeff);
    }
    case ShepardsDistortion:
    {
      /* Shepards Distortion  input arguments are the coefficents!
         Just check the number of arguments is valid!
         Args:  u1,v1, x1,y1, ...
          OR :  u1,v1, r1,g1,c1, ...
      */
      if ( number_arguments%cp_size != 0 ||
           number_arguments < cp_size ) {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
              "InvalidArgument", "%s : 'requires CP's (4 numbers each)'",
              CommandOptionToMnemonic(MagickDistortOptions, *method));
        coeff=(double *) RelinquishMagickMemory(coeff);
        return((double *) NULL);
      }
      /* User defined weighting power for Shepard's Method */
      { const char *artifact=GetImageArtifact(image,"shepards:power");
        if ( artifact != (const char *) NULL ) {
          coeff[0]=StringToDouble(artifact,(char **) NULL) / 2.0;
          if ( coeff[0] < MagickEpsilon ) {
            (void) ThrowMagickException(exception,GetMagickModule(),
                OptionError,"InvalidArgument","%s", "-define shepards:power" );
            coeff=(double *) RelinquishMagickMemory(coeff);
            return((double *) NULL);
          }
        }
        else
          coeff[0]=1.0;  /* Default power of 2 (Inverse Squared) */
      }
      return(coeff);
    }
    default:
      break;
  }
  /* you should never reach this point */
  perror("no method handler"); /* just fail assertion */
  return((double *) NULL);
}