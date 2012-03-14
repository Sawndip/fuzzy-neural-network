+ver 1.0 beta 
- 2003.5.8 제작
- Image-View 에서 가능한 기능: 'r' region select, 'right_button_down' magnification, 'a' image file save, 's' image segment save, 'f' image segment 정보 save, '1'-'3' Memory buffer 저장

I. Preprocessing
  1. Raw Image Processing 
    i. Image Filtering (Gabor, Canny, Prewitt, Sobel, Laplacian, Gaussian, High-pass)
    ii. Color Conversion (RGB -> HLS, HSV, YUV, YCrCb, XYZ, PCA(I1I2I3))
    iii. Coordinate Transform (Cartesian -> Log-Polar)
    iv. Arithmetic/Logical (Add, Sub, Mult, Div, Or, And, Xor, Not)
  2. Face Segmentation
    i. Template Matching (SQDIFF, CCORR, CCOEFF)
    ii. Blob Analysis (MAX_BLOB, LABELLING, Morphology::Erode,Dilate,Open,Close)
    iii. Threshold (In, Out, Greater, Less, Otsu)
    iv. Face Verification (...)

II. Feature Extraciton
  1. Facial Component Extraction
    i. Template Matching
    ii. Projection-based Extraction (Horiz, Vert, Both, Component Extraction)
    iii. Geometrical Estimation (...)
  2. Facial Feature Extraction
    i. Geometrical Feature (Local, Global for Eyebrows, Eyes, Mouth, NLF, NRF)
    ii. Transformed Feature (FFT/DFT)
    iii. Statistical Feature (Mean, Stddev, Norm::L1,L2,C)

III. High-level Perception
  1. Face Recognition
    i. Eigenface
  2. Facial Expression Recognition
    i. Fuzzy NN
    ii. NN
    iii. FMMNN