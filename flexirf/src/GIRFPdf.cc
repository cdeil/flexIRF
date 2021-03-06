/* ======================================================================== *\
!
 !   Author(s): Javier Rico     12/2013 <mailto:jrico@ifae.es>
 !
 !   Copyright: CTA Software Development, 2013
 !
 \* ======================================================================== */
//////////////////////////////////////////////////////////////////////////////
//
// GIRFPdf
//
// Class to represent data describing a PDF of the IRF
//
//////////////////////////////////////////////////////////////////////////////

#include "GIRFPdf.h"
#include "GIRFAxis.h"
#include "GIRFAxisBins.h"
#include "GIRFAxisParam.h"
#include "GIRFUtils.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "rootincludes.h"

using namespace std;

////////////////////////////////////////////////////////////////
// 
// Construct pdf table with type
//
GIRFPdf::GIRFPdf(PdfVar pdftype, PdfFunc pdffunc, unsigned long naxes) : fPdfFunc(pdffunc), fData(0), fIsEmpty(1) {
	fPdfVar = pdftype;
	fAxis.reserve(naxes);
}



////////////////////////////////////////////////////////////////
//
// 		Draw Pdf content
//	TODO: Just 1 and 2 dimensional IRFs can be currently drawn.
void GIRFPdf::Draw(string filename, string drawOption) const {

	if (fIsEmpty) {
		cout << "Pdf is empty" << endl;
		return;
	}
	// Check how many dimensions does the Pdf have:
	int dimensions = fAxis.size();

	if (dimensions == 0) {
		cout << "ERROR: Cannot draw empty Pdf." << endl;
		return;
	} else if (dimensions > 2){
		cout << "ERROR: Only Pdfs with dimension lower than 3 are currently supported." << endl;
		return;
	} else if (dimensions == 1){
		GIRFAxisParam* axisParam = dynamic_cast<GIRFAxisParam*>(fAxis[0]);
		GIRFAxisBins* axisBins = dynamic_cast<GIRFAxisBins*>(fAxis[0]);
		if (axisParam){

			cout << "Formula: " << axisParam->GetFormula().data() << endl;
			TF1* pdf = new TF1(GetExtName().data(), axisParam->GetFormula().data(), (float) axisParam->GetRangeMin(), (float) axisParam->GetRangeMax());
			for (int i=0;i<axisParam->GetNumPars();i++) pdf->SetParameter(i,fData[i]);
			TCanvas c1;
			TString xTitle = (TString)axisParam->GetVarName() + " [" + (TString)axisParam->GetVarUnit() + "]";
			if (axisParam->IsLog()) xTitle = "log " + (TString)axisParam->GetVarName() + " [" + (TString)axisParam->GetVarUnit() + "]";
			TString yTitle = (TString)GetVarName() + " [" + (TString)GetVarUnit() + "]";;
			pdf->SetTitle((TString)GetExtName());
			pdf->GetXaxis()->SetTitle(xTitle);
			pdf->GetYaxis()->SetTitle(yTitle);
			if (drawOption.find("logy")) c1.SetLogy();
			pdf->Draw();
			if (filename == "") c1.SaveAs("plot.png");
			else c1.SaveAs(filename.data());
			return;
		}
		if (axisBins){
			TH1F *pdf = new TH1F("pdf", GetExtName().data(), axisBins->GetSize(), axisBins->GetAxisBins().data());
			for (int i=1;i<=axisBins->GetSize();i++){
				pdf->SetBinContent(i,fData[i-1]);
			}
			TCanvas c1;
			TString xTitle = (TString)axisBins->GetVarName() + " [" + (TString)axisBins->GetVarUnit() + "]";
			TString yTitle = (TString)GetVarName() + " [" + (TString)GetVarUnit() + "]";
			if (axisBins->IsLog()) xTitle = "log " + (TString)axisBins->GetVarName() + " [" + (TString)axisBins->GetVarUnit() + "]";
			pdf->GetXaxis()->SetTitle(xTitle);
			pdf->GetYaxis()->SetTitle(yTitle);
			pdf->SetStats(0);
			std::size_t logY = drawOption.find("logY");
			if (logY!=std::string::npos) c1.SetLogy();
			pdf->Draw();
			if (filename == "") c1.SaveAs("plot.png");
			else c1.SaveAs(filename.data());
		}
	} else if (dimensions == 2){
//		GIRFAxis axisx = fAxis[0];
//		GIRFAxis axisy = fAxis[1];
//		TH2F *pdf = new TH2F("pdf", GetExtName().data(), axisx.GetSize(), GetData());
//		for (int i=1;i<=axis.GetSize();i++){
//			pdf->SetBinContent(i,fData[i-1]);
//		}
//		pdf->Draw();
		GIRFAxisParam* axisParam1 = dynamic_cast<GIRFAxisParam*>(fAxis[0]);
		GIRFAxisBins* axisBins1 = dynamic_cast<GIRFAxisBins*>(fAxis[0]);
		GIRFAxisParam* axisParam2 = dynamic_cast<GIRFAxisParam*>(fAxis[1]);
		GIRFAxisBins* axisBins2 = dynamic_cast<GIRFAxisBins*>(fAxis[1]);
		if (axisParam1 || axisParam2){
			cout << "2D Parametrized axis are not yet supported... (yet!) :)" << endl;
			return;
		}
		else if (axisBins1 && axisBins2){
			TH2F *pdf = new TH2F("pdf", GetExtName().data(), axisBins1->GetSize()-1, axisBins1->GetAxisBins().data(), axisBins2->GetSize()-1, axisBins2->GetAxisBins().data());

			for (int i=0;i<axisBins1->GetSize();i++){
				for (int j=0;j<axisBins2->GetSize();j++){
//					cout << "Data[" << i+1 << "," << j+1 << "] = fData[" << (i*(axisBins2->GetSize()))+j << "] = " << fData[(i*(axisBins1->GetSize()))+j] << endl;
					pdf->SetBinContent(i+1, j+1, fData[(i*(axisBins2->GetSize()))+j]);
				}
			}
			TCanvas c1;
			pdf->Draw();
			c1.SaveAs("plot.png");
		}
	}





}





////////////////////////////////////////////////////////////////
//
// Return the axis name for EXTNAME
//
std::string GIRFPdf::GetFuncName() const {

	string axisType;

	switch (fPdfFunc) {
	case kNumber:
		axisType = "NUM";
		break;
	case kGaussian:
		axisType = "GAUSS";
		break;
	case k2DGaussian:
		axisType = "2DGAUSS";
		break;
	default:
		cout << "Incorrect axis type.\n";
		break;
	}

	return axisType;
}

////////////////////////////////////////////////////////////////
//
// Return the axis name for EXTNAME
//
std::string GIRFPdf::GetVarName() const {

	string axisVarType;

	switch (fPdfVar) {
	case kEfficiency:
		axisVarType = "EFFIC";
		break;
	case kEDispersion:
		axisVarType = "EDISP";
		break;
	case kPsf:
		axisVarType = "PSF";
		break;
	case kBkgRate:
		axisVarType = "BGRATE";
		break;
	case kBkgRateSqDeg:
		axisVarType = "BGRATESQDEG";
		break;
	case kDiffSens:
		axisVarType = "DIFFSENS";
		break;
	case kAeff:
		axisVarType = "EFFAREA";
		break;
	case kAeffNoTheta2Cut:
		axisVarType = "EFFANOT2CUT";
		break;
	default:
		cout << "Incorrect variable type.\n";
		return axisVarType;
	}

	return axisVarType;
}

////////////////////////////////////////////////////////////////
//
// Return the axis name for EXTNAME
//
std::string GIRFPdf::GetVarUnit() const {

	//TODO: Build a separate GIRFPdf units enum, which is read/written from the FITS file.

	string pdfVarType;

	switch (fPdfVar) {
	case kEfficiency:
		pdfVarType = "";
		break;
	case kEDispersion:
		pdfVarType = "log10(TeV)";
		break;
	case kPsf:
		pdfVarType = "deg";
		break;
	case kBkgRate:
		pdfVarType = "events*s^-1";
		break;
	case kBkgRateSqDeg:
		pdfVarType = "events*s^-1*deg^-2";
		break;
	case kDiffSens:
		pdfVarType = "erg*cm^-2*s^-1";
		break;
	case kAeff:
		pdfVarType = "m^2";
		break;
	case kAeffNoTheta2Cut:
		pdfVarType = "m^2";
		break;
	default:
		cout << "Incorrect variable type.\n";
		return pdfVarType;
	}

	return pdfVarType;
}



////////////////////////////////////////////////////////////////
//
// Write the pdf and the associated axes to the specified
// file pointer
//
int GIRFPdf::Write(fitsfile* fptr, int* status) {

	// create arrays with size and first entry of every dimension to be saved (1 is first, not 0)
	int naxis = int(fAxis.size());
	long* naxes = new long[naxis];
	long* fpixel = new long[naxis];
	for (std::vector<GIRFAxis*>::size_type jaxis = 0; jaxis < naxis; jaxis++) {
		naxes[jaxis] = int(fAxis[jaxis]->GetSize());
		fpixel[jaxis] = 1;
	}
	vector<int> axisIDs;
	axisIDs.reserve(naxis);
	int axisID;
	// write associated axis blocks
	int firstaxis = 1;
	for (vector<GIRFAxis*>::iterator axis = fAxis.begin(); axis != fAxis.end();
			++axis){
		if ((*axis)->Write(fptr, axisID, status))
					cout << "GIRFPdf::Write Error: cannot write axis (error code: "
							<< *status << ")" << endl;
		else axisIDs.push_back(axisID);
	}



	// write the pdf header
	if (fits_create_img(fptr, FLOAT_IMG, naxis, naxes, status))
		cout << "GIRFPdf::Write Error: problem writing axis header (error code: "
				<< *status << ")" << endl;

	// write keywords to the header
	char keyword[9];
	char chval[20];
	char comment[70];
	ushort usval;
	// write pdf name
	sprintf(keyword, "EXTNAME");
	sprintf(chval, "%s", GetExtName().data());
	sprintf(comment, "Pdf Data HDU");
	if (fits_write_key(fptr, TSTRING, keyword, &chval, comment, status))
		cout << "GIRFPdf::Write Error: cannot write keyword (error code: "
				<< *status << ")" << endl;

	// write pdf units name
	sprintf(keyword, "BUNIT");
	sprintf(chval, "%s", GetVarUnit().data());
	sprintf(comment, "Pdf units");
		if (fits_write_key(fptr, TSTRING, keyword, &chval, comment, status))
			cout << "GIRFPdf::Write Error: cannot write keyword (error code: "
					<< *status << ")" << endl;

	int axisloop=1;
	for (vector<GIRFAxis*>::iterator axis = fAxis.begin(); axis != fAxis.end();
			++axis, axisloop++){
		sprintf(keyword, "CTYPE%d", axisloop);
		sprintf(chval, "%s", (*axis)->GetVarName().data());
		sprintf(comment, "Axis %d type", axisloop);
			if (fits_write_key(fptr, TSTRING, keyword, &chval, comment, status))
				cout << "GIRFPdf::Write Error: cannot write keyword (error code: "
						<< *status << ")" << endl;

		sprintf(keyword, "CUNIT%d", axisloop);
		sprintf(chval, "%s", (*axis)->GetVarUnit().data());
		sprintf(comment, "Axis %d units", axisloop);
			if (fits_write_key(fptr, TSTRING, keyword, &chval, comment, status))
				cout << "GIRFPdf::Write Error: cannot write keyword (error code: "
						<< *status << ")" << endl;


	}
	// write the indeces linking to the relevant axes
	for (int jaxis = 0; jaxis < naxis; jaxis++) {
		sprintf(keyword, "AXISID%d", jaxis + 1);
		usval = ushort(axisIDs[jaxis]);
		sprintf(comment, "Axis describing dimension #%d", jaxis + 1);
		if (fits_write_key(fptr, TUSHORT, keyword, &usval, comment, status))
			cout << "GIRFPdf::Write Error: cannot write keyword (error code: "
					<< *status << ")" << endl;
	}




	// write pdf var
	sprintf(keyword, "PDFVAR");
	usval = ushort(fPdfVar);
	sprintf(comment,
			"Variable whose pdf is parameterized (see GIRFPdf.h for details)");
	if (fits_write_key(fptr, TUSHORT, keyword, &usval, comment, status))
		cout << "GIRFPdf::Write Error: cannot write keyword (error code: "
				<< *status << ")" << endl;

	// write pdf function
	sprintf(keyword, "PDFFunc");
	usval = ushort(fPdfFunc);
	sprintf(comment, "Function describing pdf (see GIRFPdf.h for details)");
	if (fits_write_key(fptr, TUSHORT, keyword, &usval, comment, status))
		cout << "GIRFPdf::Write Error: cannot write keyword (error code: "
				<< *status << ")" << endl;

	// Add class keywords to the HDU.
	sprintf(keyword, "HDUCLASS");
	sprintf(chval, "CTA");
	sprintf(comment, "FITS file following the CTA data format.");
	if (fits_write_key(fptr, TSTRING, keyword, &chval, comment, status))
		cout << "GIRFAxis::WriteAxis Error: cannot write keyword (error code: "
				<< *status << ")" << endl;
	sprintf(keyword, "HDUCLAS1");
	sprintf(chval, "IRM");
	sprintf(comment, "Instrument Response Model HDU.");
	if (fits_write_key(fptr, TSTRING, keyword, &chval, comment, status))
		cout << "GIRFAxis::WriteAxis Error: cannot write keyword (error code: "
				<< *status << ")" << endl;
	sprintf(keyword, "HDUCLAS2");
	sprintf(chval, "DATA");
	sprintf(comment, "Data HDU.");
	if (fits_write_key(fptr, TSTRING, keyword, &chval, comment, status))
		cout << "GIRFAxis::WriteAxis Error: cannot write keyword (error code: "
				<< *status << ")" << endl;
	sprintf(keyword, "HDUCLAS3");
	sprintf(chval, "%s", GetVarName().data());
	sprintf(comment, "Variable whose pdf is parameterized (see GIRFPdf.h for details)");
	if (fits_write_key(fptr, TSTRING, keyword, &chval, comment, status))
		cout << "GIRFAxis::WriteAxis Error: cannot write keyword (error code: "
				<< *status << ")" << endl;
	//Get the last Pdf ID of the same class
	int pdfID = GIRFUtils::GetLastPdfID(fptr)+1;

	sprintf(keyword, "HDUCLAS4");
	usval = ushort(pdfID);
	sprintf(comment, "Pdf ID");
	if (fits_write_key(fptr, TUSHORT, keyword, &usval, comment, status))
		cout << "GIRFAxis::WriteAxis Error: cannot write keyword (error code: "
				<< *status << ")" << endl;

	// write the pdf data
	long nentries = GetSize();

//  For testing purposes:
//	float *array;
//	array = (float *) calloc(nentries+1, abs(FLOAT_IMG)/8);
//	for (int i=0;i<=nentries;i++) array[i]=fData[i];

//	if (fits_write_img(fptr, TFLOAT, 1, nentries, array, status))
//			cout << "GIRFPdf::Write Error: problem writing axis data (error code: "
//					<< *status << ")" << endl;
	if (fits_write_pix(fptr, TFLOAT, fpixel, nentries, fData, status))
		cout << "GIRFPdf::Write Error: problem writing axis data (error code: "
				<< *status << ")" << endl;

	return *status;
}



////////////////////////////////////////////////////////////////
//
// Write the pdf and the associated axes to the specified
// file pointer
//
void GIRFPdf::Print(int sanity) const {

	int iAxis=0;
	int totalSize=1;
	cout << endl;
	cout << "*******************************************" << endl;
	cout << "***          Printing GIRFPdf           ***" << endl;
	cout << "*******************************************" << endl;
	cout << "" << endl;
	cout << "Printing Axes:" << endl;
	for(std::vector<GIRFAxis*>::const_iterator axis = fAxis.begin(); axis != fAxis.end(); ++axis, iAxis++) {
		cout << "Printing Axis #" << iAxis+1 << endl;
		if (dynamic_cast<GIRFAxisParam*>(*axis)) totalSize*=(*axis)->GetSize();
		else if (dynamic_cast<GIRFAxisBins*>(*axis)) totalSize*=(*axis)->GetSize()-1;
		(*axis)->Print();
	}
	if (totalSize > sanity) cout << "Printing Pdf content (just first " << sanity << " values):" << endl;
	else cout << "Printing Pdf content:" << endl;
	int iData=0;
	// TODO: This is obviously wrong... but at least printing some numbers...

	int pdfPrintedValues=0;
	for (int i=0;i<totalSize;i++){
		if (pdfPrintedValues < sanity){
			cout << "Data[" << i+1 << "] = " << fData[i] << endl;
			pdfPrintedValues++;
		}
	}
	cout << "*******************************************" << endl;
//	for(std::vector<GIRFAxis*>::const_iterator axis = fAxis.begin(); axis != fAxis.end(); ++axis, iData++) {
//		cout << "Printing Axis #" << iData+1 << endl;
//		for (int i=0;i<(*axis)->GetSize();i++){
//			cout << fData[i] << endl;
//		}
//	}

}

void   GIRFPdf::SetData(float* data){
	if (data==0) return;
	fData = data;
	fIsEmpty=0;
}








