#pragma once

#define TEMPLATE_NAME	(L"My Transform Filter")
#define FILTER_NAME		(TEMPLATE_NAME)
#define INPUT_PIN_NAME  (L"Input")
#define OUTPUT_PIN_NAME (L"Output")

// {B5FFC4E9-66E8-46a7-9C2C-3ACC963D1D1B}
DEFINE_GUID(CLSID_MyTransform, 
0xb5ffc4e9, 0x66e8, 0x46a7, 0x9c, 0x2c, 0x3a, 0xcc, 0x96, 0x3d, 0x1d, 0x1b);

// ピンタイプの定義
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Video,        // Major type
    &MEDIASUBTYPE_RGB32      // Minor type
};

// 入力用、出力用ピンの情報
const AMOVIESETUP_PIN sudPins[] =
{
	{
		INPUT_PIN_NAME,
		FALSE,
		FALSE,
		FALSE,
		FALSE,
		&CLSID_NULL,
		NULL,
		1,
		&sudPinTypes
	},
	{
		OUTPUT_PIN_NAME,
		FALSE,
		TRUE,
		FALSE,
		FALSE,
		&CLSID_NULL,
		NULL,
		1,
		&sudPinTypes
	},
};

// フィルタ情報
const AMOVIESETUP_FILTER afFilterInfo=
{
    &CLSID_MyTransform,
    FILTER_NAME,
    MERIT_DO_NOT_USE,
    2,
    sudPins
};

// トランスフォームフィルタ
class CMyTransform : CTransformFilter {
public:
	CMyTransform(LPUNKNOWN pUnk,HRESULT *phr);
	~CMyTransform();
    static CUnknown * WINAPI	CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
	HRESULT CheckInputType(const CMediaType *mtIn);
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
	HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest);
	HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
	HRESULT CompleteConnect(PIN_DIRECTION direction, IPin *pReceivePin);
protected:
private:
	VIDEOINFOHEADER	    m_Vih;
};
