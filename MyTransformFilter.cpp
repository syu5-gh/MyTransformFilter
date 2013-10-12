#include"stdafx.h"

CMyTransform::CMyTransform(IUnknown *pUnk, HRESULT *phr) :
	CTransformFilter(FILTER_NAME, pUnk, CLSID_MyTransform)
{
}

CMyTransform::~CMyTransform() {
}

CUnknown * WINAPI CMyTransform::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr) {
	CMyTransform *pNewFilter = new CMyTransform(pUnk, phr);
	if (pNewFilter==NULL) {
		*phr=E_OUTOFMEMORY;
	}
	return pNewFilter;
}

HRESULT CMyTransform::CheckInputType(const CMediaType *mtIn) {
	if( *mtIn->Type()!=MEDIATYPE_Video || 
		*mtIn->Subtype()!=MEDIASUBTYPE_RGB32 ||
		*mtIn->FormatType()!=FORMAT_VideoInfo)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	return S_OK;
}

// 出力ピンの優先メディアタイプの提示
HRESULT CMyTransform::GetMediaType(int iPosition, CMediaType *pMediaType) {
	if(iPosition<0) {
		return E_INVALIDARG;
	}else if(iPosition>0) {
		return VFW_S_NO_MORE_ITEMS;
	}
	// 優先出力タイプは 入力メディアタイプと同じとする
	// ※ データフォーマットを変換するフィルタ(encoder,decoderなど) は異なるメディアタイプになる.
	*pMediaType=m_pInput->CurrentMediaType();
	return S_OK;
}

HRESULT CMyTransform::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut) {
	if( *mtIn->Type()!=MEDIATYPE_Video || 
		*mtIn->Subtype()!=MEDIASUBTYPE_RGB32 ||
		*mtIn->FormatType()!=FORMAT_VideoInfo)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	if(*mtIn!=*mtOut) {
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	return S_OK;
}

HRESULT CMyTransform::CompleteConnect(PIN_DIRECTION direction, IPin *pReceivePin) {
	// 入力ピンのメディアフォーマットを保存しておく
	CMediaType &type=m_pInput->CurrentMediaType();
	CopyMemory(&m_Vih,type.Format(), sizeof(VIDEOINFOHEADER));
	return S_OK;
}


//アロケータプロパティの設定
HRESULT CMyTransform::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *ppropInputRequest) {
	if(m_pInput!=NULL && m_pInput->IsConnected()==FALSE) {
		return E_FAIL;
	}
	// バッファサイズを取得し、アロケータプロパティに設定
	ppropInputRequest->cBuffers=1;
	ppropInputRequest->cbBuffer=this->m_pInput->CurrentMediaType().GetSampleSize();
	ALLOCATOR_PROPERTIES actual_prop;
	HRESULT hr=pAlloc->SetProperties(ppropInputRequest, &actual_prop );
	if(FAILED(hr)) {
		return hr;
	}
	// アロケータは要求に対して正確に一致できるとは限らないため、確保されたかチェックする
	if( ppropInputRequest->cBuffers>actual_prop.cBuffers || 
		ppropInputRequest->cbBuffer>actual_prop.cbBuffer)
	{
		return E_FAIL;
	}
	return S_OK;
}


HRESULT CMyTransform::Transform(IMediaSample *pIn, IMediaSample *pOut) {
	LPDWORD pSrc, pDest;
	pIn->GetPointer((BYTE**)&pSrc);
	pOut->GetPointer((BYTE**)&pDest);
	const long act_size=pIn->GetActualDataLength();

	for(LONG y=0;y<m_Vih.bmiHeader.biHeight;y++) {
		for(LONG x=0;x<m_Vih.bmiHeader.biWidth;x++) {
			const DWORD r = ((*pSrc )& 0xff0000) >> 16;
			const DWORD g = ((*pSrc )& 0xff00) >> 8;
			const DWORD b = (*pSrc )& 0xff;
			*pDest=0;
			*pDest= (b << 16) | (r << 8) | (g);

			pSrc++;
			pDest++;
		}
	}
	pOut->SetActualDataLength(act_size);
	pOut->SetSyncPoint(TRUE);

	LONGLONG media_start, media_end;
	pIn->GetMediaTime(&media_start, &media_end);
	pOut->SetMediaTime(&media_start, &media_end);
	REFERENCE_TIME ref_start, ref_end;
	pIn->GetTime(&ref_start, &ref_end);
	pOut->SetTime(&ref_start, &ref_end);
	pOut->SetSyncPoint(TRUE);
	return S_OK;
}
