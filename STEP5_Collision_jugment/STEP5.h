#include <d3dx9.h>

extern LPDIRECT3DDEVICE9 pDevice;

#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = NULL; } }

// ��`�̐ݒ�
// ���_���
struct CUSTOM_VERTEX
{
	// ���W
	float x, y, z, rhw;
	// ���_�̐F
	DWORD color;
	// �e�N�X�`�����W
	float tu, tv;
};

// ���������̐ݒ�
VOID InitRender()
{
	// �e�N�X�`���̐ݒ�
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
