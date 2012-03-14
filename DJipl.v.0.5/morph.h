//
// Morphology 
//	- binary
//
#ifndef _MORPHOLOGY_
class Morphology {
// structuring elements;
	unsigned char *se_buf;
	int se_width, se_height;	
	void setStructElement8(int n, int isJ = 1);
public:
	Morphology(int w=0, int h=0, unsigned char *buf = NULL);
//	Morphology(Morphology & a);
	~Morphology();
	void setStructuringElement(int w, int h, unsigned char *buf);
	unsigned char *getStructuringElement() {return se_buf;}
	int 	w() {return se_width;}
	int 	h() {return se_height;}
	unsigned char &pel(int x, int y) {return se_buf[x + y * se_width];}
	BinaryImage &dilation(BinaryImage &A, BinaryImage &C);
	BinaryImage &erosion(BinaryImage &A, BinaryImage &C);
	BinaryImage &opening(BinaryImage &A, BinaryImage &C);
	BinaryImage &closing(BinaryImage &A, BinaryImage &C);

	// Made by DJ.
	// Ref. D. Chai's paper.f
	GrayImage &dilation2(GrayImage &A, GrayImage &C, int mmx, int mx, int my, int mth);
	GrayImage &erosion2(GrayImage &A, GrayImage &C, int mmx, int mx, int my, int mth);
	BinaryImage &dilation3(BinaryImage &A, BinaryImage &C, int mmx, int mx, int my, int mth);
	BinaryImage &erosion3(BinaryImage &A, BinaryImage &C, int mmx, int mx, int my, int mth);

	// Dense map.
	GrayImage &denseMap(BinaryImage &A, GrayImage &C, int sx, int sy, int mx, int my);

	// Basic Morphological Algorithms
	void complement();
/*
	BinaryImage &complement(BinaryImage &A, BinaryImage &C)
								{ C = ~A; return C; }
*/
	BinaryImage &intersection(BinaryImage &A,BinaryImage &B,  BinaryImage &C)
								{ C = A & B; return C; }
	BinaryImage &Union(BinaryImage &A, BinaryImage &B, BinaryImage &C)
								{ C = A | B; return C; }
	void reflection();
/*
	BinaryImage &translation(BinaryImage &A, BinaryImage &C);
*/
	BinaryImage &difference(BinaryImage &A, BinaryImage &B, BinaryImage &C)
								{ C = A & (~B); return C; }
	BinaryImage &filling(int x, int y, BinaryImage &A, BinaryImage &C, int n = -1);
	BinaryImage &HitMiss(BinaryImage &A, BinaryImage &C, int w1 = 1, int h1 = 0);
	BinaryImage &HitMiss(BinaryImage &A, BinaryImage &C, Morphology &J, Morphology &K);
	BinaryImage &thickening(BinaryImage &A, BinaryImage &C, int iterations = -1);
	BinaryImage &thinning(BinaryImage &A, BinaryImage &C, int iterations = -1);
/*
	BinaryImage &boundaryExtraction(BinaryImage &A, BinaryImage &C);
	BinaryImage &connectedComponent(BinaryImage &A, BinaryImage &C);
	BinaryImage &convexhull(BinaryImage &A, BinaryImage &C);
	BinaryImage &skeleton(BinaryImage &A, BinaryImage &C);
	BinaryImage &pruning(BinaryImage &A, BinaryImage &C);
*/
	Morphology&  operator = (const Morphology &m);
	void  print(FILE *fout = stdout, char *name = NULL);
};
#endif
