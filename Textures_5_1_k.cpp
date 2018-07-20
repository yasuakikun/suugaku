//------------------------------------------------------------
// Textures_5_1.cpp
// �������̋��`��(Z�o�b�t�@��肠��o�[�W����)
// 
//------------------------------------------------------------

#include <D3D11.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include<d3d11shader.h>
#include<DirectXTex\DirectXTex.h>
#include<WICTextureLoader\WICTextureLoader.h>

#define VIEW_WIDTH			800					// ��ʕ�
#define VIEW_HEIGHT			600					// ��ʍ���

#define PI					3.1415927f			// �~����
#define ROT_SPEED			( PI / 100.0f )		// ��]���x
#define R					2.0f				// �}�`�̑傫��
#define R_2					1.0f				// �}�`�̑傫��
#define CORNER_NUM			20					// �p��

using namespace DirectX;

// ���_�\����
struct CUSTOMVERTEX {
    XMFLOAT4	v4Pos;
	XMFLOAT2	v2UV;
};
// �e�N�X�`���G�\����
struct TEX_PICTURE {
	ID3D11ShaderResourceView	*pSRViewTexture;
	D3D11_TEXTURE2D_DESC		tdDesc;
	int							nWidth, nHeight;
};

TEX_PICTURE				g_tTexture;
TEX_PICTURE				g_tTextureHutomei;


// �e�N�X�`���t���C���f�b�N�X3D�|���S���̕`��
int DrawIndexed3DPolygonsTex( CUSTOMVERTEX *pVertices, int nVertexNum, WORD *pIndices, int nIndexNum,ID3D11ShaderResourceView* srv );

// ���[���h�s��̐���
XMMATRIX CreateWorldMatrix( void )
{
	float			fAngleY;							// y�������]�p
	static float	fAngleX = 0.0f;						// x�������]�p
	XMMATRIX		matRot_Y;							// y�������]�s��
	XMMATRIX		matRot_X;							// x�������]�s��

	// ������]
	fAngleY = 2.0f * PI * ( float )( timeGetTime() % 3000 ) / 3000.0f;

	// �L�[���͂ŉ�]
	if ( GetAsyncKeyState( VK_UP ) ) {
		fAngleX += ROT_SPEED;
	}
	if ( GetAsyncKeyState( VK_DOWN ) ) {
		fAngleX -= ROT_SPEED;
	}

	// �s��쐬
	matRot_Y = XMMatrixRotationY( fAngleY );
	matRot_X = XMMatrixRotationX( fAngleX );

	return matRot_Y * matRot_X;						// ��]�̍���
}


// ���[���h�s��̐���
XMMATRIX CreateWorldMatrix2( void )
{
	float			fAngleY;							// y�������]�p
	static float	fAngleX = 0.0f;						// x�������]�p
	static float x=0;
	static float z=0;

	XMMATRIX		matRot_Y;							// y�������]�s��
	XMMATRIX		matRot_X;							// x�������]�s��

	// ������]
	fAngleY = 2.0f * PI * ( float )( timeGetTime() % 3000 ) / 3000.0f;


	// �L�[���͂ŉ�]
	if ( GetAsyncKeyState( VK_LEFT ) ) {
		fAngleX += ROT_SPEED;
	}
	if ( GetAsyncKeyState( VK_RIGHT ) ) {
		fAngleX -= ROT_SPEED;
	}

	// �L�[���͂ňړ�
	if ( GetAsyncKeyState( 'W' ) ) {
		z+=0.1;
	}
	if ( GetAsyncKeyState( 'A' ) ) {
		x-=0.1;
	}

	if ( GetAsyncKeyState( 'S' ) ) {
		z-=0.1;
	}
	if ( GetAsyncKeyState( 'D' ) ) {
		x+=0.1;
	}

	// �s��쐬
	matRot_Y = XMMatrixRotationY( fAngleY );
	matRot_X = XMMatrixRotationX( fAngleX );

	

	return matRot_Y * matRot_X*XMMatrixTranslation(x,0,z);
}



// �摜�̕`��
int DrawChangingPictures( void )
{
	int					i, j;
	CUSTOMVERTEX		Vertices[( CORNER_NUM + 1 ) * ( CORNER_NUM / 4 + 1 )];	// ���_�f�[�^
	WORD				wIndices[CORNER_NUM * CORNER_NUM / 4 * 2 * 3];	// �C���f�b�N�X�f�[�^
	float				fTheta;
	float				fPhi;
	float				fAngleDelta;
	int					nIndex;						// �f�[�^�̃C���f�b�N�X
	int					nIndexY;					// x�����C���f�b�N�X

	// ���_�f�[�^�쐬
	fAngleDelta = 2.0f * PI / CORNER_NUM;
	nIndex = 0;
	fTheta = 0.0f;
	for ( i = 0; i < CORNER_NUM / 4 + 1; i++ ) {
		fPhi = 0.0f;
		for ( j = 0; j < CORNER_NUM + 1; j++ ) {
			Vertices[nIndex].v4Pos  = XMFLOAT4( R * sinf( fTheta ) * cosf( fPhi ),
												R * cosf( fTheta ),
												R * sinf( fTheta ) * sinf( fPhi ), 1.0f );
			Vertices[nIndex].v2UV = XMFLOAT2( fPhi / ( 2.0f * PI ), fTheta / PI );
			nIndex++;
			fPhi += fAngleDelta;
		}
		fTheta += fAngleDelta;
	}

	// �C���f�b�N�X�f�[�^�쐬
	nIndex = 0;
	for ( i = 0; i < CORNER_NUM; i++ ) {
		for ( j = 0; j < CORNER_NUM / 4; j++ ) {
			nIndexY = j * ( CORNER_NUM + 1 );
			wIndices[nIndex    ] = nIndexY + i;
			wIndices[nIndex + 1] = nIndexY + ( CORNER_NUM + 1 ) + i;
			wIndices[nIndex + 2] = nIndexY + i + 1;
			nIndex += 3;
			wIndices[nIndex    ] = nIndexY + i + 1;
			wIndices[nIndex + 1] = nIndexY + ( CORNER_NUM + 1 ) + i;
			wIndices[nIndex + 2] = nIndexY + ( CORNER_NUM + 1 ) + i + 1;
			nIndex += 3;
		}
	}

	// ���_�C���f�b�N�X���g���ĕ`��
	DrawIndexed3DPolygonsTex( Vertices, ( CORNER_NUM + 1 ) * ( CORNER_NUM / 4 + 1 ),
							  wIndices, CORNER_NUM * CORNER_NUM / 4 * 2 * 3 ,g_tTexture.pSRViewTexture);

	return 0;
}



// �摜�̕`��
int DrawChangingPictures2( void )
{
	int					i, j;
	CUSTOMVERTEX		Vertices[( CORNER_NUM + 1 ) * ( CORNER_NUM / 4 + 1 )];	// ���_�f�[�^
	WORD				wIndices[CORNER_NUM * CORNER_NUM / 4 * 2 * 3];	// �C���f�b�N�X�f�[�^
	float				fTheta;
	float				fPhi;
	float				fAngleDelta;
	int					nIndex;						// �f�[�^�̃C���f�b�N�X
	int					nIndexY;					// x�����C���f�b�N�X

	// ���_�f�[�^�쐬
	fAngleDelta = 2.0f * PI / CORNER_NUM;
	nIndex = 0;
	fTheta = 0.0f;
	for ( i = 0; i < CORNER_NUM / 4 + 1; i++ ) {
		fPhi = 0.0f;
		for ( j = 0; j < CORNER_NUM + 1; j++ ) {
			Vertices[nIndex].v4Pos  = XMFLOAT4( R_2 * sinf( fTheta ) * cosf( fPhi ),
												R_2 * cosf( fTheta ),
												R_2 * sinf( fTheta ) * sinf( fPhi ), 1.0f );
			Vertices[nIndex].v2UV = XMFLOAT2( fPhi / ( 2.0f * PI ), fTheta / PI );
			nIndex++;
			fPhi += fAngleDelta;
		}
		fTheta += fAngleDelta;
	}

	// �C���f�b�N�X�f�[�^�쐬
	nIndex = 0;
	for ( i = 0; i < CORNER_NUM; i++ ) {
		for ( j = 0; j < CORNER_NUM / 4; j++ ) {
			nIndexY = j * ( CORNER_NUM + 1 );
			wIndices[nIndex    ] = nIndexY + i;
			wIndices[nIndex + 1] = nIndexY + ( CORNER_NUM + 1 ) + i;
			wIndices[nIndex + 2] = nIndexY + i + 1;
			nIndex += 3;
			wIndices[nIndex    ] = nIndexY + i + 1;
			wIndices[nIndex + 1] = nIndexY + ( CORNER_NUM + 1 ) + i;
			wIndices[nIndex + 2] = nIndexY + ( CORNER_NUM + 1 ) + i + 1;
			nIndex += 3;
		}
	}

	// ���_�C���f�b�N�X���g���ĕ`��
	DrawIndexed3DPolygonsTex( Vertices, ( CORNER_NUM + 1 ) * ( CORNER_NUM / 4 + 1 ),
							  wIndices, CORNER_NUM * CORNER_NUM / 4 * 2 * 3 ,g_tTextureHutomei.pSRViewTexture);

	return 0;
}


//------------------------------------------------------------
// �ȉ��ADirectX�ɂ��\���v���O����

#include <stdio.h>
#include <windows.h>
#include <tchar.h>								// Unicode�E�}���`�o�C�g�����֌W


#define MAX_BUFFER_VERTEX				10000	// �ő�o�b�t�@���_��
#define MAX_BUFFER_INDEX				20000	// �ő�o�b�t�@�C���f�b�N�X��


// �����N���C�u����
#pragma comment( lib, "d3d11.lib" )   // D3D11���C�u����
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "directxtex.lib" )
#pragma comment( lib, "winmm.lib" )


// �Z�[�t�����[�X�}�N��
#ifndef SAFE_RELEASE
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release(); ( p )=NULL; } }
#endif

// �V�F�[�_�萔�\����
struct CBNeverChanges
{
    XMMATRIX mView;
};


// �O���[�o���ϐ�
UINT  g_nClientWidth;							// �`��̈�̉���
UINT  g_nClientHeight;							// �`��̈�̍���

HWND        g_hWnd;         // �E�B���h�E�n���h��


ID3D11Device			*g_pd3dDevice;			// �f�o�C�X
IDXGISwapChain			*g_pSwapChain;			// DXGI�X���b�v�`�F�C��
ID3D11DeviceContext		*g_pImmediateContext;	// �f�o�C�X�R���e�L�X�g
ID3D11RasterizerState	*g_pRS;					// ���X�^���C�U
ID3D11RenderTargetView	*g_pRTV;				// �����_�����O�^�[�Q�b�g
ID3D11Texture2D*        g_pDepthStencil = NULL;	// Z�o�b�t�@
ID3D11DepthStencilView* g_pDepthStencilView = NULL;	// Z�o�b�t�@�̃r���[
ID3D11DepthStencilState *g_pDSStencilState = NULL;	// Z�o�b�t�@�̃X�e�[�g
ID3D11DepthStencilState *g_pDSStencilState2 = NULL;	// Z�o�b�t�@�̃X�e�[�g
D3D_FEATURE_LEVEL       g_FeatureLevel;			// �t�B�[�`���[���x��

ID3D11Buffer			*g_pVertexBuffer;		// ���_�o�b�t�@
ID3D11Buffer			*g_pIndexBuffer;		// �C���f�b�N�X�o�b�t�@
ID3D11BlendState		*g_pbsAddBlend;		// ���Z�u�����h
ID3D11BlendState		*g_pbsNormalBlend;		// �ӂ��u�����h
ID3D11VertexShader		*g_pVertexShader;		// ���_�V�F�[�_
ID3D11PixelShader		*g_pPixelShader;		// �s�N�Z���V�F�[�_
ID3D11InputLayout		*g_pInputLayout;		// �V�F�[�_���̓��C�A�E�g
ID3D11SamplerState		*g_pSamplerState;		// �T���v���X�e�[�g

ID3D11Buffer			*g_pCBNeverChanges = NULL;

// �`�撸�_�o�b�t�@
CUSTOMVERTEX g_cvVertices[MAX_BUFFER_VERTEX];
int							g_nVertexNum = 0;

WORD		g_wIndices[MAX_BUFFER_INDEX];
int							g_nIndexNum = 0;



// ���݂̃e�N�X�`��
ID3D11ShaderResourceView	*g_pNowTexture = NULL;

void SetDepthStencilState(){

	// �X�e���V���X�e�[�g�̍쐬
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	HRESULT hr = g_pd3dDevice->CreateDepthStencilState( &dsDesc, &g_pDSStencilState );

	g_pImmediateContext->OMSetDepthStencilState( g_pDSStencilState, 1 );
}
// Direct3D�̏�����
HRESULT InitD3D( void )
{
    HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL  FeatureLevelsRequested[6] = { D3D_FEATURE_LEVEL_11_0,
													 D3D_FEATURE_LEVEL_10_1,
													 D3D_FEATURE_LEVEL_10_0,
													 D3D_FEATURE_LEVEL_9_3,
													 D3D_FEATURE_LEVEL_9_2,
													 D3D_FEATURE_LEVEL_9_1 };
	UINT               numLevelsRequested = 6;
	D3D_FEATURE_LEVEL  FeatureLevelsSupported;

	// �f�o�C�X�쐬
	hr = D3D11CreateDevice( NULL,
					D3D_DRIVER_TYPE_HARDWARE, 
					NULL, 
					0,
					FeatureLevelsRequested, 
					numLevelsRequested,
					D3D11_SDK_VERSION, 
					&g_pd3dDevice,
					&FeatureLevelsSupported,
					&g_pImmediateContext );
	if( FAILED ( hr ) ) {
		return hr;
	}

	// �t�@�N�g���̎擾
	IDXGIDevice * pDXGIDevice;
	hr = g_pd3dDevice->QueryInterface( __uuidof( IDXGIDevice ), ( void ** )&pDXGIDevice );
	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent( __uuidof( IDXGIAdapter ), ( void ** )&pDXGIAdapter );
	IDXGIFactory * pIDXGIFactory;
	pDXGIAdapter->GetParent( __uuidof( IDXGIFactory ), ( void ** )&pIDXGIFactory);

	// �X���b�v�`�F�C���̍쐬
    DXGI_SWAP_CHAIN_DESC	sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = g_nClientWidth;
	sd.BufferDesc.Height = g_nClientHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	hr = pIDXGIFactory->CreateSwapChain( g_pd3dDevice, &sd, &g_pSwapChain );

	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pIDXGIFactory->Release();

	if( FAILED ( hr ) ) {
		return hr;
	}

    // �����_�����O�^�[�Q�b�g�̐���
    ID3D11Texture2D			*pBackBuffer = NULL;
    D3D11_TEXTURE2D_DESC BackBufferSurfaceDesc;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't get backbuffer." ), _T( "Error" ), MB_OK );
        return hr;
    }
    pBackBuffer->GetDesc( &BackBufferSurfaceDesc );
    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRTV );
    SAFE_RELEASE( pBackBuffer );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't create render target view." ), _T( "Error" ), MB_OK );
        return hr;
    }

    // *** Create depth stencil texture ***
    D3D11_TEXTURE2D_DESC descDepth;
	RECT rc;
    GetClientRect( g_hWnd, &rc );
	ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = rc.right - rc.left;
    descDepth.Height = rc.bottom - rc.top;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // *** Create the depth stencil view ***
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

	// *** �����_�����O�^�[�Q�b�g�ݒ� ***
    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, NULL );
    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, g_pDepthStencilView );

    // ���X�^���C�U�̐ݒ�
    D3D11_RASTERIZER_DESC drd;
	ZeroMemory( &drd, sizeof( drd ) );
	drd.FillMode				= D3D11_FILL_SOLID;
	drd.CullMode				= D3D11_CULL_NONE;
	drd.FrontCounterClockwise	= FALSE;
	drd.DepthClipEnable			= TRUE;
    hr = g_pd3dDevice->CreateRasterizerState( &drd, &g_pRS );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't create rasterizer state." ), _T( "Error" ), MB_OK );
        return hr;
    }
    g_pImmediateContext->RSSetState( g_pRS );

    // �r���[�|�[�g�̐ݒ�
    D3D11_VIEWPORT vp;
    vp.Width    = ( FLOAT )g_nClientWidth;
    vp.Height   = ( FLOAT )g_nClientHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    return S_OK;
}


// �v���O���}�u���V�F�[�_�쐬
HRESULT MakeShaders( void )
{
    HRESULT hr;
    ID3DBlob* pVertexShaderBuffer = NULL;
    ID3DBlob* pPixelShaderBuffer = NULL;
    ID3DBlob* pError = NULL;

    DWORD dwShaderFlags = 0;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	// �R���p�C��
	hr = D3DCompileFromFile(_T("Basic_3D_Tex.fx"), nullptr, nullptr, "VS", "vs_4_0_level_9_1",
		dwShaderFlags, 0, &pVertexShaderBuffer, &pError);
	if (FAILED(hr)) {
		MessageBox(NULL, _T("Can't open Basic_3D_Tex.fx"), _T("Error"), MB_OK);
		SAFE_RELEASE(pError);
		return hr;
	}
	hr = D3DCompileFromFile(_T("Basic_3D_Tex.fx"), nullptr, nullptr, "PS", "ps_4_0_level_9_1",
		dwShaderFlags, 0, &pPixelShaderBuffer, &pError);
    if ( FAILED( hr ) ) {
        SAFE_RELEASE( pVertexShaderBuffer );
        SAFE_RELEASE( pError );
        return hr;
    }
    SAFE_RELEASE( pError );
    
    // VertexShader�쐬
    hr = g_pd3dDevice->CreateVertexShader( pVertexShaderBuffer->GetBufferPointer(),
										   pVertexShaderBuffer->GetBufferSize(),
										   NULL, &g_pVertexShader );
    if ( FAILED( hr ) ) {
        SAFE_RELEASE( pVertexShaderBuffer );
        SAFE_RELEASE( pPixelShaderBuffer );
        return hr;
    }
    // PixelShader�쐬
    hr = g_pd3dDevice->CreatePixelShader( pPixelShaderBuffer->GetBufferPointer(),
										  pPixelShaderBuffer->GetBufferSize(),
										  NULL, &g_pPixelShader );
    if ( FAILED( hr ) ) {
        SAFE_RELEASE( pVertexShaderBuffer );
        SAFE_RELEASE( pPixelShaderBuffer );
        return hr;
    }

    // ���̓o�b�t�@�̓��͌`��
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	UINT numElements = ARRAYSIZE( layout );
	// ���̓o�b�t�@�̓��͌`���쐬
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements,
										  pVertexShaderBuffer->GetBufferPointer(),
										  pVertexShaderBuffer->GetBufferSize(),
										  &g_pInputLayout );
    SAFE_RELEASE( pVertexShaderBuffer );
    SAFE_RELEASE( pPixelShaderBuffer );
    if ( FAILED( hr ) ) {
        return hr;
    }

    // �V�F�[�_�萔�o�b�t�@�쐬
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof( bd ) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( CBNeverChanges );
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBNeverChanges );
    if( FAILED( hr ) )
        return hr;

	// �ϊ��s��
    CBNeverChanges	cbNeverChanges;
	XMMATRIX		mScreen;
    mScreen = XMMatrixIdentity();
	mScreen.r[0].m128_f32[0] = 2.0f / g_nClientWidth;
	mScreen.r[1].m128_f32[1] = -2.0f / g_nClientHeight;
	mScreen.r[3].m128_f32[0] = -1.0f;
	mScreen.r[3].m128_f32[1] = 1.0f;
	cbNeverChanges.mView = XMMatrixTranspose( mScreen );
	g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );

    return S_OK;
}


// �e�N�X�`�����[�h
int LoadTexture(TCHAR *szFileName, TEX_PICTURE *pTexPic, int nWidth, int nHeight,
	int nTexWidth, int nTexHeight)
{

	ID3D11Resource *resource = nullptr;
	HRESULT						hr;
	ID3D11Texture2D				*pTexture;


	hr = DirectX::CreateWICTextureFromFile(g_pd3dDevice, g_pImmediateContext, szFileName, &resource, &(pTexPic->pSRViewTexture));

	if (FAILED(hr)) {
		return hr;
	}
	pTexPic->pSRViewTexture->GetResource((ID3D11Resource **)&(pTexture));
	pTexture->GetDesc(&(pTexPic->tdDesc));
	pTexture->Release();

	pTexPic->nWidth = nWidth;
	pTexPic->nHeight = nHeight;

	return S_OK;
}



// �`�惂�[�h�I�u�W�F�N�g������
int InitDrawModes( void )
{
    HRESULT				hr;

	// �u�����h�X�e�[�g
	D3D11_BLEND_DESC BlendDesc;
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable           = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice->CreateBlendState( &BlendDesc, &g_pbsAddBlend );
	if ( FAILED( hr ) ) {
		return hr;
	}


	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable           = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice->CreateBlendState( &BlendDesc, &g_pbsNormalBlend );
	if ( FAILED( hr ) ) {
		return hr;
	}


	// �T���v��
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof( samDesc ) );
	samDesc.Filter          = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU        = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV        = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW        = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.ComparisonFunc  = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD          = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState( &samDesc, &g_pSamplerState );
	if ( FAILED( hr ) ) {
		return hr;
	}

	return S_OK;
}


// �W�I���g���̏�����
HRESULT InitGeometry( void )
{
    HRESULT hr = S_OK;

    // ���_�o�b�t�@�쐬
    D3D11_BUFFER_DESC BufferDesc;
    BufferDesc.Usage                = D3D11_USAGE_DYNAMIC;
    BufferDesc.ByteWidth            = sizeof( CUSTOMVERTEX ) * MAX_BUFFER_VERTEX;
    BufferDesc.BindFlags            = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags       = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags            = 0;

    D3D11_SUBRESOURCE_DATA SubResourceData;
    SubResourceData.pSysMem             = g_cvVertices;
    SubResourceData.SysMemPitch         = 0;
    SubResourceData.SysMemSlicePitch    = 0;
    hr = g_pd3dDevice->CreateBuffer( &BufferDesc, &SubResourceData, &g_pVertexBuffer );
    if ( FAILED( hr ) ) {
        return hr;
    }

    // �C���f�b�N�X�o�b�t�@�쐬
    BufferDesc.Usage                = D3D11_USAGE_DYNAMIC;
    BufferDesc.ByteWidth            = sizeof( WORD ) * MAX_BUFFER_INDEX;
    BufferDesc.BindFlags            = D3D11_BIND_INDEX_BUFFER;
    BufferDesc.CPUAccessFlags       = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags            = 0;

	SubResourceData.pSysMem         = g_wIndices;
    hr = g_pd3dDevice->CreateBuffer( &BufferDesc, &SubResourceData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

	// �e�N�X�`���쐬
	g_tTexture.pSRViewTexture =  NULL;
	hr = LoadTexture( _T( "6.bmp" ), &g_tTexture, 512, 512, 512, 512 );
    if ( FAILED( hr ) ) {
 		MessageBox( NULL, _T( "Can't open 6.bmp" ), _T( "Error" ), MB_OK );
       return hr;
    }
    g_tTextureHutomei.pSRViewTexture=nullptr;
    hr = LoadTexture( _T( "umaru.png" ), &g_tTextureHutomei, 512, 512, 512, 512 );
    if ( FAILED( hr ) ) {
 		MessageBox( NULL, _T( "Can't open hutomei.png" ), _T( "Error" ), MB_OK );
       return hr;
    }
	return S_OK;
}


// �I������
int Cleanup( void )
{
    SAFE_RELEASE( g_tTexture.pSRViewTexture );
    SAFE_RELEASE( g_pVertexBuffer );
    SAFE_RELEASE( g_pIndexBuffer );

    SAFE_RELEASE( g_pSamplerState );
    SAFE_RELEASE( g_pbsAddBlend );
    SAFE_RELEASE( g_pInputLayout );
    SAFE_RELEASE( g_pPixelShader );
    SAFE_RELEASE( g_pVertexShader );
    SAFE_RELEASE( g_pCBNeverChanges );

    SAFE_RELEASE( g_pRS );									// ���X�^���C�U

	// �X�e�[�^�X���N���A
	if ( g_pImmediateContext ) {
		g_pImmediateContext->ClearState();
		g_pImmediateContext->Flush();
	}

    SAFE_RELEASE( g_pRTV );									// �����_�����O�^�[�Q�b�g
    SAFE_RELEASE( g_pDepthStencil );						// Z�o�b�t�@
    SAFE_RELEASE( g_pDepthStencilView );					// Z�o�b�t�@�̃r���[
    SAFE_RELEASE( g_pDSStencilState );						// Z�o�b�t�@�̃X�e�[�g

    // �X���b�v�`�F�[��
    if ( g_pSwapChain != NULL ) {
        g_pSwapChain->SetFullscreenState( FALSE, 0 );
    }
    SAFE_RELEASE( g_pSwapChain );

    SAFE_RELEASE( g_pImmediateContext );					// �f�o�C�X�R���e�L�X�g
    SAFE_RELEASE( g_pd3dDevice );							// �f�o�C�X

	return 0;
}


// �E�B���h�E�v���V�[�W��
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


// �G�̕`��҂��s��t���b�V��
int FlushDrawingPictures( void )
{
	HRESULT			hr;

	if ( ( g_nVertexNum > 0 ) && g_pNowTexture ) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = g_pImmediateContext->Map( g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		if ( SUCCEEDED( hr ) ) {
			CopyMemory( mappedResource.pData, &( g_cvVertices[0] ), sizeof( CUSTOMVERTEX ) * g_nVertexNum );
			g_pImmediateContext->Unmap( g_pVertexBuffer, 0 );
			hr = g_pImmediateContext->Map( g_pIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
			if ( SUCCEEDED( hr ) ) {
				CopyMemory( mappedResource.pData, &( g_wIndices[0] ), sizeof( WORD ) * g_nIndexNum );
				g_pImmediateContext->Unmap( g_pIndexBuffer, 0 );
				g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pNowTexture );
				g_pImmediateContext->DrawIndexed( g_nIndexNum, 0, 0 );
			}
		}
	}
	g_nVertexNum = 0;
	g_nIndexNum = 0;

	return 0;
}


// �e�N�X�`���t���C���f�b�N�X3D�|���S���̕`��
int DrawIndexed3DPolygonsTex( CUSTOMVERTEX *pVertices, int nVertexNum, WORD *pIndices, int nIndexNum ,ID3D11ShaderResourceView* srv)
{
	int				i;

	CopyMemory( &g_cvVertices[0], pVertices, sizeof( CUSTOMVERTEX ) * nVertexNum );
	for ( i = 0; i < nIndexNum; i++ ) {
		g_wIndices[g_nIndexNum + i] = *( pIndices + i ) + g_nVertexNum;
	}

	g_nVertexNum += nVertexNum;
	g_nIndexNum  += nIndexNum;

	g_pNowTexture = srv;

	return 0;
}


// �����_�����O
HRESULT Render( void )
{
    // ��ʃN���A
	XMFLOAT4	v4Color = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
    g_pImmediateContext->ClearRenderTargetView( g_pRTV, ( float * )&v4Color );
	// *** Z�o�b�t�@�N���A ***
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    // �T���v���E���X�^���C�U�Z�b�g
    g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerState );
    g_pImmediateContext->RSSetState( g_pRS );
    
    // �`��ݒ�
    UINT nStrides = sizeof( CUSTOMVERTEX );
    UINT nOffsets = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &nStrides, &nOffsets );
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    g_pImmediateContext->IASetInputLayout( g_pInputLayout );

    // �V�F�[�_�ݒ�
    g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
    g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );

	// �ϊ��s��
    CBNeverChanges	cbNeverChanges;
	XMMATRIX		mWorld;
	XMMATRIX		mView;
	XMMATRIX		mProjection;


	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet( 0.0f, 3.0f, -5.0f, 0.0f );
	XMVECTOR At = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	mView = XMMatrixLookAtLH( Eye, At, Up );

    // Initialize the projection matrix
	mProjection = XMMatrixPerspectiveFovLH( XM_PIDIV4, VIEW_WIDTH / ( FLOAT )VIEW_HEIGHT, 0.01f, 100.0f );

	g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, g_pDepthStencilView );

	//�s�����`��
	g_pImmediateContext->OMSetBlendState( NULL, NULL, 0xFFFFFFFF );

	DrawChangingPictures2();

	mWorld = CreateWorldMatrix2();
	cbNeverChanges.mView = XMMatrixTranspose( mWorld * mView * mProjection );
	g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );

	FlushDrawingPictures();

	
    
	g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, NULL );


	// �������`��
	g_pImmediateContext->OMSetBlendState( g_pbsAddBlend, NULL, 0xFFFFFFFF );
	DrawChangingPictures();

	mWorld = CreateWorldMatrix();
	cbNeverChanges.mView = XMMatrixTranspose( mWorld * mView * mProjection );
	g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );

	
	



    // �\��
	FlushDrawingPictures();

    return S_OK;
}


// �G���g���|�C���g
int WINAPI _tWinMain( HINSTANCE hInst, HINSTANCE, LPTSTR, int )
{
	LARGE_INTEGER			nNowTime, nLastTime;		// ���݂ƂЂƂO�̎���
	LARGE_INTEGER			nTimeFreq;					// ���ԒP��

    // ��ʃT�C�Y
    g_nClientWidth  = VIEW_WIDTH;						// ��
    g_nClientHeight = VIEW_HEIGHT;						// ����

	HRESULT hr = CoInitializeEx(NULL,
		COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// Register the window class
    WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
                      _T( "D3D Sample" ), NULL };
    RegisterClassEx( &wc );

	RECT rcRect;
	SetRect( &rcRect, 0, 0, g_nClientWidth, g_nClientHeight );
	AdjustWindowRect( &rcRect, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( _T( "D3D Sample" ), _T( "Textures_5_1" ),
						   WS_OVERLAPPEDWINDOW, 100, 20, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top,
						   GetDesktopWindow(), NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D() ) && SUCCEEDED( MakeShaders() ) )
    {
        // Create the shaders
        if( SUCCEEDED( InitDrawModes() ) )
        {
			if ( SUCCEEDED( InitGeometry() ) ) {					// �W�I���g���쐬

				// Show the window
				ShowWindow( g_hWnd, SW_SHOWDEFAULT );
				UpdateWindow( g_hWnd );
				
				QueryPerformanceFrequency( &nTimeFreq );			// ���ԒP��
				QueryPerformanceCounter( &nLastTime );				// 1�t���[���O����������

				// Enter the message loop
				MSG msg;
				ZeroMemory( &msg, sizeof( msg ) );
				while( msg.message != WM_QUIT )
				{
					Render();
					do {
						if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
						{
							TranslateMessage( &msg );
							DispatchMessage( &msg );
						}
						QueryPerformanceCounter( &nNowTime );
					} while( ( ( nNowTime.QuadPart - nLastTime.QuadPart ) < ( nTimeFreq.QuadPart / 90 ) ) &&
							 ( msg.message != WM_QUIT ) );
					while( ( ( nNowTime.QuadPart - nLastTime.QuadPart ) < ( nTimeFreq.QuadPart / 60 ) ) &&
						   ( msg.message != WM_QUIT ) )
					{
						QueryPerformanceCounter( &nNowTime );
					}
					nLastTime = nNowTime;
					g_pSwapChain->Present( 0, 0 );					// �\��
				}
			}
        }
    }

    // Clean up everything and exit the app
    Cleanup();
    UnregisterClass( _T( "D3D Sample" ), wc.hInstance );
    return 0;
}
