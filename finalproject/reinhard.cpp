/*
  Landon Byrd
	11/12/2018
	This program takes an image's look and feel and maps it onto another image using
	the Rheinard method. See more in README
*/

#include <cmath>
#include "matrix.h"

#ifndef PI
#define PI 3.1415926536
#endif

#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <OpenImageIO/imageio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "pixel.h"

using namespace std;

OIIO_NAMESPACE_USING

//data structure to hold pixel data
string infilename;


//global initializations to more easily save data between functions
RGBA_pixel *rgba_pixels;
RGBA_pixel **rgba_pixels_2D;

RGBA_pixel *rgba_pixels2;
RGBA_pixel **rgba_pixels_2D2;

int xres;
int yres;

int xres2;
int yres2;

int nchannels;
int nchannels2;
int iter = 0;

//clamps between two values
//used to clamp rgb values between 0 and 255
int clamp(int input, int low, int high){
	if(input<low){
		input =low;
	} else if(input>high){
		input = high;
	}
	return input;
}

/*
	Reshape Callback Routine: sets up the viewport and drawing coordinates
    This routine is called when the window is created and every time the window
	is resized, by the program or by the user
*/
void handleReshape(int w, int h) {
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// define the drawing coordinate system on the viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);// sets up a 2D orthographic viewing region
}
//flips Image stored in rgba_pixels
void flipImage(){

		for(int i =0; i<xres; i++){
			for(int j =0; j<yres/2;j++){
				RGBA_pixel temp;
				temp = rgba_pixels_2D[j][i];
				rgba_pixels_2D[j][i] = rgba_pixels_2D[yres-j-1][i];
				rgba_pixels_2D[yres-j-1][i] = temp;
			}
		}
}

//flips second image
void flipImage2(){

		for(int i =0; i<xres2; i++){
			for(int j =0; j<yres2/2;j++){
				RGBA_pixel temp;
				temp = rgba_pixels_2D2[j][i];
				rgba_pixels_2D2[j][i] = rgba_pixels_2D2[yres2-j-1][i];
				rgba_pixels_2D2[yres2-j-1][i] = temp;
			}
		}
}

//this takes in an image name that it opens up, flips right side up,
//and stores in rgba_pixels for further processing
void OpenImage(string ImageName){
	ImageInput* imagedata = ImageInput::open(ImageName);
 if(!imagedata){
	 cerr <<"File does not exist"<<endl;
	 return;
 }

 const ImageSpec &spec = imagedata->spec();
 xres = spec.width;
 yres = spec.height;
 nchannels = spec.nchannels;

unsigned char *pixels;
 pixels = new unsigned char [xres*yres*nchannels];
 imagedata->read_image(TypeDesc::UINT8, &pixels[0]);
 delete rgba_pixels;
 rgba_pixels = new RGBA_pixel[xres*yres];

switch (nchannels) {
	case 1:
		for(int i = 0; i<xres*yres; i++){
			rgba_pixels[i].r = pixels[i];
			rgba_pixels[i].g = pixels[i];
			rgba_pixels[i].b = pixels[i];
			rgba_pixels[i].a = 255;
		}
	break;
	case 3:
			for(int i = 0; i<xres*yres; i++){
				rgba_pixels[i].r = pixels[i*nchannels];
				rgba_pixels[i].g = pixels[i*nchannels+1];
				rgba_pixels[i].b = pixels[i*nchannels+2];
				rgba_pixels[i].a = 255;
			}
	case 4:
		for(int i = 0; i<xres*yres; i++){
			rgba_pixels[i].r = pixels[i*nchannels];
			rgba_pixels[i].g = pixels[i*nchannels+1];
			rgba_pixels[i].b = pixels[i*nchannels+2];
			rgba_pixels[i].a = pixels[i*nchannels+3];
		}
	break;
	default:
		std::cout <<"Error"<<std::endl;
}

//using 2D for efficient access
rgba_pixels_2D = new RGBA_pixel*[yres];
rgba_pixels_2D[0] = rgba_pixels;
for(int i=1; i<yres;i++){
	rgba_pixels_2D[i] = rgba_pixels_2D[i-1]+xres;
}
	flipImage();
	imagedata->close();
 	ImageInput::destroy(imagedata);
}

 //this takes in an image name that it opens up, flips right side up,
 //and stores in rgba_pixels for further processing
void OpenImage2(string ImageName){
 	ImageInput* imagedata = ImageInput::open(ImageName);
  if(!imagedata){
 	 cerr <<"File does not exist"<<endl;
 	 return;
  }



  const ImageSpec &spec2 = imagedata->spec();
  xres2 = spec2.width;
  yres2 = spec2.height;
  nchannels2 = spec2.nchannels;

unsigned char *pixels;
 pixels = new unsigned char [xres2*yres2*nchannels2];
 imagedata->read_image(TypeDesc::UINT8, &pixels[0]);
 delete rgba_pixels2;
 rgba_pixels2 = new RGBA_pixel[xres2*yres2];

switch (nchannels2) {
	case 1:
		for(int i = 0; i<xres2*yres2; i++){
			rgba_pixels2[i].r = pixels[i];
			rgba_pixels2[i].g = pixels[i];
			rgba_pixels2[i].b = pixels[i];
			rgba_pixels2[i].a = 255;
		}
	break;
	case 3:
			for(int i = 0; i<xres2*yres2; i++){
				rgba_pixels2[i].r = pixels[i*nchannels2];
				rgba_pixels2[i].g = pixels[i*nchannels2+1];
				rgba_pixels2[i].b = pixels[i*nchannels2+2];
				rgba_pixels2[i].a = 255;
			}
	case 4:
		for(int i = 0; i<xres2*yres2; i++){
			rgba_pixels2[i].r = pixels[i*nchannels2];
			rgba_pixels2[i].g = pixels[i*nchannels2+1];
			rgba_pixels2[i].b = pixels[i*nchannels2+2];
			rgba_pixels2[i].a = pixels[i*nchannels2+3];
		}
	break;
	default:
		std::cout <<"Error"<<std::endl;
}

//using 2D for efficient access
rgba_pixels_2D2 = new RGBA_pixel*[yres2];
rgba_pixels_2D2[0] = rgba_pixels2;
for(int i=1; i<yres2;i++){
	rgba_pixels_2D2[i] = rgba_pixels_2D2[i-1]+xres2;
}

	flipImage2();

	imagedata->close();

 	ImageInput::destroy(imagedata);
}


//this calls the gl functions necessary to write rgb_pixels to screen
//in a window that is appropriately sized
void DrawImage(){

  // specify window clear (background) color to be opaque white
  glClearColor(0, 0, 0, 1);

  // clear window to background color
  glClear(GL_COLOR_BUFFER_BIT);

	glRasterPos2i(0,0);

	glDrawPixels(xres,yres,GL_RGBA, GL_UNSIGNED_BYTE, rgba_pixels);
  // flush the OpednGL pipeline to the viewport

  glFlush();
}



void WriteImage(string outfilename){

  // create the oiio file handler for the image
  ImageOutput *outfile = ImageOutput::create(outfilename);
  if(!outfile){
    cerr << "Could not create output image for " << outfilename << ", error = " << geterror() << endl;
    return;
  }

	//set up file for writing to
  ImageSpec spec(xres, yres, 4, TypeDesc::UINT8);
  if(!outfile->open(outfilename, spec)){
    cerr << "Could not open " << outfilename << ", error = " << geterror() << endl;
    ImageOutput::destroy(outfile);
    return;
  }
	//flip Image for storage
	flipImage();

  if(!outfile->write_image(TypeDesc::UINT8, rgba_pixels)){
    cerr << "Could not write image to " << outfilename << ", error = " << geterror() << endl;
    ImageOutput::destroy(outfile);
    return;
  }
  else
    cout << outfilename<<" is stored" << endl;

  if(!outfile->close()){
    cerr << "Could not close " << outfilename << ", error = " << geterror() << endl;
    ImageOutput::destroy(outfile);
    return;
  }

	//unflip Image for Display
	flipImage();

  delete outfile;
}



/*
  Keyboard Callback Routine: 'w' to write to file, 'q' or ESC quit,
	'r' to open a new image
  This routine is called every time a key is pressed on the keyboard
*/
void handleKey(unsigned char key, int x, int y){
  switch(key){
    case 'r':		// read in a new image file
    case 'R':
			cout<<"Enter a new file to display: "<<endl;
			cin>>infilename;
			OpenImage(infilename);
			glutReshapeWindow(xres,yres);
      glutPostRedisplay();
      break;

		case 'w': 	// write a new image file
		case 'W':
			cout<<"Enter a file to write the fixed image to"<<endl;
			cin>>infilename;
			WriteImage(infilename);

		break;

    case 'q':		// q - quit
    case 'Q':
    case 27:		// esc - quit
      exit(0);

		case 'c':
		case 'C':
			glutPostRedisplay();

    default:		// not a valid key -- just ignore it
      return;
  }
}


//prototypes
Vector3D RGBtoLAB(Vector3D);
Vector3D LABtoRGB(Vector3D);

//converts Vector3D to RGBA pixel type
RGBA_pixel Vector3DtoPixel(Vector3D vec){
	RGBA_pixel pix;
	pix.r = int(vec.x);
	pix.g = int(vec.y);
	pix.b = int(vec.z);
	pix.a = 255;

	return pix;
}

//converts RGBApixel type to Vector3D
Vector3D PixeltoVector3D(RGBA_pixel pix){
	return Vector3D(int(pix.r),int(pix.g),int(pix.b));
}

//pass in pixel
//return a vector a lab deviations and means in out params
void LABCalculations(RGBA_pixel **pixels, int xdim, int ydim, Vector3D &standard_dev, Vector3D &means){
	int total =0;
	//through every pixel
	for(int i = 0; i<ydim;i++){
		for(int j = 0; j<xdim;j++){
			Vector3D inc = RGBtoLAB(PixeltoVector3D(pixels[i][j]));
			means = means + inc;
			total++;
		}
	}

	means = means/total;

	//through every pixel
	for(int i = 0; i<ydim;i++){
		for(int j = 0; j<xdim;j++){
			Vector3D vec_pix =  RGBtoLAB(PixeltoVector3D(pixels[i][j]));
			vec_pix = vec_pix - means;
			vec_pix = Vector3D(pow(vec_pix.x,2), pow(vec_pix.y,2), pow(vec_pix.z,2));
			standard_dev = standard_dev + vec_pix;
		}
	}
	standard_dev = standard_dev/total;
	standard_dev = Vector3D(sqrt(standard_dev.x), sqrt(standard_dev.y), sqrt(standard_dev.z));
}

Vector3D CorrectPixel(Vector3D pixel, Vector3D orig_dev, Vector3D orig_means, Vector3D new_dev, Vector3D new_means){
	Vector3D LabVal = RGBtoLAB(pixel);

	//map new color onto old
	//See Reinhard
	LabVal = LabVal - orig_means;
	LabVal= LabVal*(new_dev/orig_dev);
	LabVal = LabVal + new_means;
	LabVal = LABtoRGB(LabVal);

	//clamp values so colors don't mess up
	LabVal.x = clamp(LabVal.x, 0,255);
	LabVal.y = clamp(LabVal.y, 0,255);
	LabVal.z = clamp(LabVal.z, 0,255);
	return LabVal;
}


//convert RGB to LMS space
Vector3D RGBtoLMS(Vector3D RGBval){

	//RGB to LMS conversion matrix
	double by3[3][3] = {
												{.3811, .5783, .0402},
												{.1967, .7244, .0782},
												{.0241, .1288, .8444}
											};
	Matrix3D RGB_To_LMS_Mat(by3);

	Vector3D LMSval = RGB_To_LMS_Mat * RGBval;

	//convert to log space if not 0
	if(LMSval.x != 0)LMSval.x = log10(LMSval.x);
	if(LMSval.y != 0)LMSval.y = log10(LMSval.y);
	if(LMSval.z != 0)LMSval.z = log10(LMSval.z);
	return LMSval;
}

//convert LMS to LAB space
Vector3D LMStoLAB(Vector3D LMSval){

	//decorrelate l, a, and b
	double by3[3][3] = {
									{1/sqrt(3), 0, 0},
									{0, 1/sqrt(6), 0},
									{0, 0, 1/sqrt(2)}
								};
	Matrix3D LMS_To_RGB_Mat1(by3);

	double by3_2[3][3] =  {
										{1, 1, 1},
										{1, 1, -2},
										{1, -1, 0}
									};

	Matrix3D LMS_To_RGB_Mat2(by3_2);

	Vector3D LABval = LMS_To_RGB_Mat1 * LMS_To_RGB_Mat2 *LMSval;
	return LABval;
}


//convert RGB to LAB space
Vector3D RGBtoLAB(Vector3D RGBval){
	Vector3D val =  LMStoLAB(RGBtoLMS(RGBval));
	return val;
}


//convert LAB to LMS space
Vector3D LABtoLMS(Vector3D Labval){
	//undo decorrelation
	double by3[3][3] = {
											 {1, 1, 1},
											 {1, 1, -1},
											 {1, -2, 0}
										 };
	Matrix3D LAB_To_LMS_Mat1(by3);

	double by3_2[3][3] = {
													{sqrt(3)/3, 0, 0},
													{0, sqrt(6)/6, 0},
													{0, 0, sqrt(2)/2}
												};

	Matrix3D LAB_To_LMS_Mat2(by3_2);

  Vector3D LMSval = LAB_To_LMS_Mat1 * LAB_To_LMS_Mat2 *Labval;

	return LMSval;

}

//convert LMS to RGB space
Vector3D LMStoRGB(Vector3D LMSval){
	LMSval.x = pow(10,LMSval.x);
	LMSval.y = pow(10,LMSval.y);
	LMSval.z = pow(10,LMSval.z);
	double by3[3][3] = {
												{4.4679, -3.5873, .1193},
												{-1.2186, 2.3809, -.1624},
												{.0497, -.2439, 1.2045}
											};
	Matrix3D LMS_To_RGB_Mat(by3);

	Vector3D RGBval = LMS_To_RGB_Mat* LMSval;
	return RGBval;

}

//convert LAB to RGB
Vector3D LABtoRGB(Vector3D LABval){
	return LMStoRGB(LABtoLMS(LABval));
}

/*
   Main program to draw the square, change colors, and wait for quit
*/
int main(int argc, char* argv[]){

  // start up the glut utilities
  glutInit(&argc, argv);

	//check to see more than just executable argument
	if(argc >=3){

		//open both images into different arrays
		OpenImage(argv[1]);
		OpenImage2(argv[2]);

		//get LAB standards deviations and means for first image
		Vector3D standard_dev1, means1;
		LABCalculations(rgba_pixels_2D,xres,yres, standard_dev1, means1);

		//get LAB standard deviations and means for second image
		Vector3D standard_dev2, means2;
		LABCalculations(rgba_pixels_2D2,xres2, yres2, standard_dev2, means2);

		//through all pixels in first image
		for(int i = 0; i<xres; i++){
			for(int j=0; j<yres; j++){

				//convert RGBApixel to Vector3D
				Vector3D vForm= PixeltoVector3D(rgba_pixels_2D[j][i]);

				//return Rheinard shifted pixel from values
				Vector3D cVec = CorrectPixel(vForm, standard_dev1, means1, standard_dev2, means2);

				//set pixel in image to the new values
				rgba_pixels_2D[j][i] =  Vector3DtoPixel(cVec);
			}
		}

	} else {
		cerr<<"Too few arguments"<<endl;
		exit(1);
	}

  // create the graphics window, giving width, height, and title text
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  glutInitWindowSize(xres,yres);
  glutCreateWindow("Original");

  // an event
  glutDisplayFunc(DrawImage);	  // display callback
  glutKeyboardFunc(handleKey);	  // keyboard callback
  glutReshapeFunc(handleReshape); // window resize callback

  glutMainLoop();
  return 0;
}
