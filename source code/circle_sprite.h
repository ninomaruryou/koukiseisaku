#pragma once

#include <d3d11.h>
#include <directxmath.h>

class CircleSprite
{
public:


	ID3D11VertexShader* vertex_shader; //���_�V�F�[�_�[�̃C���^�[�t�F�[�X�i���̃V�X�e���ƘA�����邽�߁H�j�Ǘ��V�X�e��
	ID3D11PixelShader* pixel_shader; //�s�N�Z���V�F�[�_�[�ȉ����i�X�e�[�W�𐧌䂷��Ǘ��V�X�e���j
	ID3D11InputLayout* input_layout; //���̓��C�A�E�g���������i�A�b�Z���u���[�̑��荞�݂̒�`�j
	ID3D11Buffer* vertex_buffer; //�o�b�t�@�[�����i���_�V�F�[�_�[�̃f�[�^���i�[�j
	ID3D11Buffer* index_buffer; //�C���f�b�N�X�o�b�t�@�i�C���f�b�N�X�V�F�[�_�[�̃f�[�^���i�[�j

	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a // ��`�̃T�C�Y�i�X�N���[�����W�n�j
		, float angle);

	/*�f�X�g���N�^�ł͂��ׂĂ� COM �I�u�W�F�N�g���������*/

	CircleSprite(ID3D11Device* device);
	~CircleSprite();


	static constexpr UINT NUM_CIRCLE_POINTS = 600; //�~�̒��_�����萔
};



//struct vertex //���_�̐ݒ�
//{
//	DirectX::XMFLOAT3 position;
//	DirectX::XMFLOAT4 color;
//};
