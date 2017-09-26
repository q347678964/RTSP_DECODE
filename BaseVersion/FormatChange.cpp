#include "stdafx.h"
#include "resource.h"

#include "FormatChange.h"
#include <string>

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


wchar_t* FormatChange::AnsiToUnicode( const char* szStr )
{
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );
	if (nLen == 0)
	{
	   return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );
	return pResult;
}

char* FormatChange::UnicodeToAnsi( const wchar_t* szStr )
{
	int nLen = WideCharToMultiByte( CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL );
	if (nLen == 0)
	{
	   return NULL;
	}
	char* pResult = new char[nLen];
	WideCharToMultiByte( CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL );
	return pResult;
}

char* FormatChange::CStringToChar(CString cstring )
{
	return this->UnicodeToAnsi(cstring.GetBuffer(0));
}

CString FormatChange::GetTimeCString(void){
	CTime time = CTime::GetCurrentTime();   ///构造CTime对象
	CString TempCString = time.Format("%Y%m%d_%H%M");
	return TempCString;
}