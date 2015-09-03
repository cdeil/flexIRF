/* ======================================================================== *\
!
!   Author(s): Javier Rico     12/2013 <mailto:jrico@ifae.es>
!
!   Copyright: CTA Software Development, 2013
!
\* ======================================================================== */

//////////////////////////////////////////////////////////////////////////////
//
// GIRF
//
// Class to hold, read/write CTA IRF 
//
//////////////////////////////////////////////////////////////////////////////


#include "GIRF.h"
#include "fitsio.h"
#include "GIRFConfig.h"
#include "GIRFPdf.h"


using namespace std;

////////////////////////////////////////////////////////////////
// 
// Construct fits object: open new file and overwrite
// its content if already exists
// Use only for empty Primary Image fits file
//
GIRF::GIRF() 
{
}


////////////////////////////////////////////////////////////////
// 
// Write IRF into file, return status (0 if ok)
// (use filename="-" for stdout, or
// start it with "!" to force overwriting)
// 
int GIRF::Write(string filename) 
{
  fitsfile* fptr; // pointer to output file
  int status=0;   // must be initialized (0 means ok)
  
  // create output file 
  if(fits_create_file(&fptr,filename.data(),&status))
    cout << "GIRF::Write Error: cannot create file (error code: " << status <<")" << endl;
  
  // write primary HDU
  if(fits_create_img(fptr,BYTE_IMG,0,NULL,&status))
    cout << "GIRF::Write Error: cannot write primary header (error code: " << status <<")" << endl;
  
  // write extra keywords (so far just an example)
  char keyword[] = "ORIGIN";
  char keyval [] = "CTA Consortium";
  char comment[] = "Organization responsible";
  if(fits_write_key(fptr,TSTRING,keyword,&keyval,comment,&status))
     cout << "GIRF::Write Error: cannot write keyword (error code: " << status <<")" << endl;
      
  // write pdf blocks and associated axes
  int ipdf=1;
  int iaxis=1;
  for(vector<GIRFPdf*>::iterator pdf=fPdfList.begin();pdf!=fPdfList.end();++pdf,ipdf++)
    if((*pdf)->Write(fptr,ipdf,iaxis,&status))
      cout << "GIRF::Write Error: cannot write pdf (error code: " << status <<")" << endl;

  // close output file
  if(fits_close_file(fptr,&status))
    cout << "GIRF::Write Error: cannot close file (error code: " << status <<")" << endl;

  return status;
}

GIRFPdf GIRF::GetPdf(GIRFPdf::PdfVar pdfVar, GIRFConfig config){

	GIRFPdf extractedPdf;





	return extractedPdf;

}




