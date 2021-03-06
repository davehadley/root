/// Generates the ImageName output of the macro MacroName

#include <stdio.h>

void makeimage(const char *MacroName, const char *ImageName, const char *OutDir, bool cp, bool py)
{
   gStyle->SetImageScaling(3.);

   // Execute the macro as a C++ one or a Python one.
   if (!py) gROOT->ProcessLine(Form(".x %s",MacroName));
   else     gROOT->ProcessLine(Form("TPython::ExecScript(\"%s\");",MacroName));

   // If needed, copy the macro in the documentation directory.
   if (cp) {
      TString MN = MacroName;
      Int_t i = MN.Index("(");
      Int_t l = MN.Length();
      if (i>0) MN.Remove(i, l);
      gSystem->Exec(TString::Format("cp %s %s/macros", MN.Data(), OutDir));
   }

   // Iterate over all the pads generated by the macro and produce the corresponding pictures.
   TIter iCanvas(gROOT->GetListOfCanvases());
   TCanvas* canvas = 0;
   int ImageNum = 0;

   FILE *s = fopen("ImagesSizes.dat", "w");

   int cw;
   while ((canvas = (TCanvas*) iCanvas())) {
      ImageNum++;
      canvas->SaveAs(TString::Format("%s/html/pict%d_%s",OutDir,ImageNum,ImageName));
      cw = canvas->GetWindowWidth();
      fprintf(s,"%d\n",cw);
   }

   fclose(s);

   FILE *f = fopen("NumberOfImages.dat", "w");
   fprintf(f,"%d\n",ImageNum);
   fclose(f);
}
