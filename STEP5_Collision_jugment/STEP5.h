#include <d3dx9.h>

extern LPDIRECT3DDEVICE9 pDevice;

#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } }

// 矩形の設定
// 頂点情報
struct CUSTOM_VERTEX
{
	// 座標
	float x, y, z, rhw;
	// 頂点の色
	DWORD color;
	// テクスチャ座標
	float tu, tv;
};

// 透明処理の設定
VOID InitRender()
{
	// テクスチャの設定
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
