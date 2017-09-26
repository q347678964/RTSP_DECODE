#ifndef FORMAT_CHANGE
#define FORMAT_CHANGE


class FormatChange {

public:
	FormatChange();
	~FormatChange();
	void CImage2CBitmap(CImage &Image,CBitmap &Bitmap);
	void FormatChange::CBitmap2Hbitmap(CBitmap &Bitmap,HBITMAP &Hbitmap);
	void FormatChange::Hbitmap2CBitmap(HBITMAP &Hbitmap,CBitmap &Bitmap);
	wchar_t* FormatChange::AnsiToUnicode( const char* szStr );
	char* FormatChange::UnicodeToAnsi( const wchar_t* szStr );
	char* FormatChange::CStringToChar(CString cstring );

	CString FormatChange::GetTimeCString(void);
};
#endif