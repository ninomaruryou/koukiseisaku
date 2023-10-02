#include "COM_obj_.h"
#include "misc.h"

void States::Initialize(ID3D11Device* device, ID3D11DeviceContext* immediate_context)
{
	HRESULT hr{ S_OK }; //�G���[���e�H ���݂͐���

	
	{
		// ���L�R�[�h�͐[�x�e�X�g�F�I���A�[�x���C�g�F�I���̐ݒ�ł���
		// �[�x�e�X�g�F�I���E�I�t�A�[�x���C�g�F�I���E�I�t�̑S�S�ʂ�̃I�u�W�F�N�g

		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{}; //�f�v�X�X�e���V���r���[
		depth_stencil_desc.DepthEnable = TRUE; //�[�x�e�X�g
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; //�[�x���C�g
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	{
		//�A framework �N���X�� initialize �����o�֐��ŃT���v���[�X�e�[�g�I�u�W�F�N�g�𐶐�����
		D3D11_SAMPLER_DESC sampler_desc; //�T���v���[�X�e�[�g(�T���v�����O�̃C���^�[�t�F�[�X�H)�`�揈�����Ԉ����悤�Ȃ���
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //�ŏ����A�g��A�~�b�v���x���T���v�����O�ɂ̓|�C���g�T���v�����O���g�p
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//�e�N�X�`�����W�����������@		�e�N�X�`�����^�C����i���ʂɁH�����ɁH�j���܂�
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;// //�e�N�X�`���̋��E�O	����
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; //�e�N�X�`���̋��E�O	����
		sampler_desc.MipLODBias = 0; //float�^ �ǉ��~�b�v�}�b�v�W���l�f�o�b�N�p�Ƃ��Ŏg������
		sampler_desc.MaxAnisotropy = 16; //clamp�Ɏg�p(�ő�l�ŏ��l�̂悤�Ȃ���)
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampler_desc, sampler_states.emplace_back().ReleaseAndGetAddressOf()); //�e�N�X�`���̃T���v���[��Ԃ��쐬
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		  sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//�e�N�X�`���̃T���v�����O���@ (�k���A�g��A�~�b�v���x�� �T���v�����O�ɐ��`��Ԃ��g�p)
		  hr = device->CreateSamplerState(&sampler_desc, sampler_states.emplace_back().ReleaseAndGetAddressOf());//�e�N�X�`���̃T���v�����O�����J�v�Z��������I�u�W�F�N�g���쐬
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = device->CreateSamplerState(&sampler_desc, sampler_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// SHADOW
		sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; //D3D11_COMPARISON_LESS_EQUAL
		sampler_desc.BorderColor[0] = 1;
		sampler_desc.BorderColor[1] = 1;
		sampler_desc.BorderColor[2] = 1;
		sampler_desc.BorderColor[3] = 1;
		hr = device->CreateSamplerState(&sampler_desc, sampler_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//UNIT10
//// �T���v���X�e�[�g�̐���
//
//		sampler_desc = {};
//		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
//		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
//		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
//		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
//		sampler_desc.MipLODBias = 0;
//		sampler_desc.MaxAnisotropy = 16;
//		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//		sampler_desc.BorderColor[0] = FLT_MAX;
//		sampler_desc.BorderColor[1] = FLT_MAX;
//		sampler_desc.BorderColor[2] = FLT_MAX;
//		sampler_desc.BorderColor[3] = FLT_MAX;
//		sampler_desc.MinLOD = 0;
//		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
//		hr = device->CreateSamplerState(&sampler_desc, shadowmap_sampler_state.GetAddressOf());
//

	}

	{
		//�Aframework �N���X�̃R���X�g���N�^�Ńu�����h�X�e�[�g�I�u�W�F�N�g���쐬����
		D3D11_BLEND_DESC blend_desc{};

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, blend_states.emplace_back().GetAddressOf()); //blend_states�ɂ��܂�Ӗ��Ȃ�
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		blend_desc = {};

		//���Z
		  //blend_desc.AlphaToCoverageEnable = FALSE;
		  //blend_desc.IndependentBlendEnable = FALSE;
		  //blend_desc.RenderTarget[0].BlendEnable = TRUE;
		  //blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		  //blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		  //blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		  //blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		  //blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		  //blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		  //blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		  //hr = device->CreateBlendState(&blend_desc, blend_states[1].GetAddressOf());
		  //_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	//���X�^���C�Y�̐ݒ�
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;//�O�p�`���\�������w�ʂ������肵�܂������v���ł��`�ʂ��Ă����J�����O
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states.emplace_back().GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FrontCounterClockwise = TRUE;//�O�p�`���\�������w�ʂ������肵�܂������v���ł��`�ʂ��Ă����J�����O

	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states.emplace_back().GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states.emplace_back().GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states.emplace_back().GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


}
