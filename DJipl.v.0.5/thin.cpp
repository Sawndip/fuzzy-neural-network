/* Zhang-Suen thinning */

int y [256][256];

main()
{

	int i,j,k,n,m;
	int im[256][256];


/*	Read the glyph							*/
	scanf ("%d", &n);	scanf("%d", &m);
	for (i=0; i<n; i++) {
	   for (j=0; j<m; j++) {
		scanf ("%d", &k);
		im[i][j] = k;
	}  }

	iputim (im, n, m);
	thin_b (im, n, m);
	iputim (im, n, m);
}

iputim (image,n,m)
	int image[256][256],n,m;
{
	int i,j;

	for (i=0; i<n; i++) {
	   for (j=0; j<m; j++)  {
		if (image[i][j] >= 100) image[i][j] = 9;
		printf ("%3d", image[i][j]);
	   }
	   printf ("\n");
	}
}


thin_b (image,nn,mm)
	int image[256][256], nn,mm;
{
/*		Thinning algorithm: CACM 1984 march (Zhang and Suen)	*/

	int i,j,n,m,k, cont, br,ar,p1,p2, t1a();
	int a[8];

	printf ("Thinning algorithm: CACM 1984 march (Zhang and Suen)	\n");
	cont = 1;
	while (cont) {
		printf ("+\n");
		cont = 0;

/*	Sub-iteration 1: */
		for (i=0; i<nn; i++)
		  for (j=0; j<mm; j++) {		/* Scan the entire image */
			if (image[i][j] == 0) {
				y[i][j] = 0;
				continue;
			}
			ar = t1a (image, i, j, a, &br,nn,mm);	/* Function A */
			p1 = a[0]*a[2]*a[4];
			p2 = a[2]*a[4]*a[6];
			if ( (ar == 1) && ((br>=2) && (br<=6)) &&
				(p1 == 0) && (p2 == 0) )  {
					y[i][j] = 1;
					cont = 1;
			}
			else y[i][j] = 0;
		}
		subtr (y, image,nn,mm);

/* Sub iteration 2: */
		for (i=0; i<nn; i++)
		  for (j=0; j<mm; j++) {		/* Scan the entire image */
			if (image[i][j] == 0) {
				y[i][j] = 0;
				continue;
			}
			ar = t1a (image, i, j, a, &br,nn,mm);	/* Function A */
			p1 = a[0]*a[2]*a[6];
			p2 = a[0]*a[4]*a[6];
			if ( (ar == 1) && ((br>=2) && (br<=6)) &&
				(p1 == 0) && (p2 == 0) )  {
					y[i][j] = 1;
					cont = 1;
			}
			else y[i][j] = 0;
		}
		subtr (y, image,nn,mm);
	}
}

subtr (a, b, n,m)
	int a[256][256], b[256][256],n,m;
{
	int i,j;

	for (i=0; i<n; i++)
		for (j=0; j<m; j++) 
			b[i][j] -= a[i][j];
}


int t1a (image, i, j, a, b,nn,mm)
	int image[256][256];
	int i,j, *b,nn,mm;
	int a[8];
{
/*	Return the number of 01 patterns in the sequence of pixels
	P2 p3 p4 p5 p6 p7 p8 p9.					*/

	int n,m;

	for (n=0; n<8; n++) a[n] = 0;
	if (i-1 >= 0) {
		a[0] = image[i-1][j];
		if (j+1 < mm) a[1] = image[i-1][j+1];
		if (j-1 >= 0) a[7] = image[i-1][j-1];
	}
	if (i+1 < nn) {
		a[4] = image[i+1][j];
		if (j+1 < mm) a[3] = image[i+1][j+1];
		if (j-1 >= 0) a[5] = image[i+1][j-1];
	}
	if (j+1 < mm) a[2] = image[i][j+1];
	if (j-1 >= 0) a[6] = image[i][j-1];

	m= 0;	*b = 0;
	for (n=0; n<7; n++) {
		if ((a[n]==0) && (a[n+1]==1)) m++;
		*b = *b + a[n];
	}
	if ((a[7] == 0) && (a[0] == 1)) m++;
	*b = *b + a[7];
	return m;
}

/*		End of method B					*/

