// PGMimage.h: interface for the PGMimage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PGMIMAGE_H__FC51899B_D02A_4743_8E87_015F9D062981__INCLUDED_)
#define AFX_PGMIMAGE_H__FC51899B_D02A_4743_8E87_015F9D062981__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct {
  char *name;
  int rows, cols;
  int *data;
} IMAGE;

typedef struct {
  int n;
  IMAGE **list;
} IMAGELIST;

class PGMimage  
{
public:
	PGMimage();
	virtual ~PGMimage();


	/*** User accessible macros ***/

	#define ROWS(img)  ((img)->rows)
	#define COLS(img)  ((img)->cols)
	#define NAME(img)   ((img)->name)

	/*** User accessible functions ***/

	IMAGE *img_alloc();
	IMAGE *img_creat(char *name, int nr, int nc);
	void img_free(IMAGE *img);
	void img_setpixel(IMAGE *img, int r, int c, int val);
	int img_getpixel(IMAGE *img, int r, int c);
	IMAGE *img_open(char *filename);
	int img_write(IMAGE *img, char *filename);

	void imgl_add(IMAGELIST *il, IMAGE *img);
	void imgl_free(IMAGELIST *il);
	void imgl_load_images_from_textfile(IMAGELIST *il, char *filename);
	IMAGELIST *imgl_alloc();

	char *img_basename(char *filename);
	void imgl_munge_name(char *buf);
};

#endif // !defined(AFX_PGMIMAGE_H__FC51899B_D02A_4743_8E87_015F9D062981__INCLUDED_)
