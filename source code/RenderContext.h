#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

//	UV�X�N���[�����
struct UVScrollData
{
	DirectX::XMFLOAT2	uvScrollValue;	// UV�X�N���[���l
};

// �}�X�N�f�[�^
struct MaskData
{
	ID3D11ShaderResourceView* maskTexture;
	float					dissolveThreshold;
	float					edgeThreshold; 	// ����臒l
	DirectX::XMFLOAT4			edgeColor;		// ���̐F
};

//	���s�������
struct DirectionalLightData
{
	DirectX::XMFLOAT4	direction;	// ����
	DirectX::XMFLOAT4	color;		// �F
};

// �_�������
struct PointLightData
{
	DirectX::XMFLOAT4	position;	// ���W
	DirectX::XMFLOAT4	color;		// �F
	float			range;		// �͈�
	DirectX::XMFLOAT3	dummy;
};

// �_�����̍ő吔
static	constexpr	int	PointLightMax = 8;

// �X�|�b�g���C�g���
struct SpotLightData
{
	DirectX::XMFLOAT4	position;	// ���W
	DirectX::XMFLOAT4	direction;	// ����
	DirectX::XMFLOAT4	color;		// �F
	float			range;		// �͈�
	float			innerCorn; 	// �C���i�[�p�x�͈�
	float			outerCorn; 	// �A�E�^�[�p�x�͈�
	float			dummy;
};

// �X�|�b�g���C�g�̍ő吔
static	constexpr	int	SpotLightMax = 8;

// �����_�[�R���e�L�X�g
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;

	//	�J�������
	DirectX::XMFLOAT4		viewPosition;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;

	//	�X�N���[���f�[�^
	UVScrollData			uvScrollData;
	// �}�X�N�f�[�^
	MaskData				maskData;



	//	���C�g���
	DirectX::XMFLOAT4		ambientLightColor;
	DirectionalLightData	directionalLightData;
	PointLightData		pointLightData[PointLightMax];	// �_�������
	int					pointLightCount = 0;			// �_������
	SpotLightData			spotLightData[SpotLightMax];	// �X�|�b�g���C�g���
	//int					pointLightCount = 0;			// �_������
	int					spotLightCount = 0;			// �X�|�b�g���C�g��



};
