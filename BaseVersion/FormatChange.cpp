#include "stdafx.h"
#include "resource.h"

#include "FormatChange.h"


FormatChange::FormatChange(){

}
FormatChange::~FormatChange(){

}
void FormatChange::CImage2CBitmap(CImage &Image,CBitmap &Bitmap){
	HBITMAP hbmp =(HBITMAP)Image.operator HBITMAP();		//CImage -> Hbitmap
	this->Hbitmap2CBitmap(hbmp,Bitmap);				//Hbitmap->Cbitmap
}

void FormatChange::Hbitmap2CBitmap(HBITMAP &Hbitmap,CBitmap &Bitmap){		//Hbitmap->Cbitmap
	Bitmap.DeleteObject();
	Bitmap.Attach(Hbitmap);
}

void FormatChange::CBitmap2Hbitmap(CBitmap &Bitmap,HBITMAP &Hbitmap){		//Cbitmap -> Hbitmap
	Hbitmap=(HBITMAP)Bitmap;
}
