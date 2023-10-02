#pragma once


#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <wrl/client.h>
//#include "texture.h"

using namespace Microsoft::WRL;

struct vertex;

class Sprite_Batch
{
	//�o�b�t�@�[�Ƃ̓f�[�^�̕ۑ��Ȃǂ��菕��������̂�������GPU��p�̃������H�i�ɏՑ́H�j

	//�ϐ��͂��ׂăC���^�[�t�F�[�X�̂悤�Ȃ��́H
	ComPtr<ID3D11VertexShader> vertex_shader; //���_�V�F�[�_�[�̃C���^�[�t�F�[�X�i���̃V�X�e���ƘA�����邽�߁H�j�Ǘ��V�X�e��
	ComPtr<ID3D11PixelShader> pixel_shader; //�s�N�Z���V�F�[�_�[�i�X�e�[�W�𐧌䂷��Ǘ��V�X�e���j
	ComPtr<ID3D11InputLayout> input_layout; //���̓��C�A�E�g�i�A�b�Z���u���[�̑��荞�݂̒�`�j
	ComPtr<ID3D11Buffer> vertex_buffer; //�o�b�t�@�[�i���_�V�F�[�_�[�̃f�[�^���i�[�j

	ComPtr<ID3D11ShaderResourceView> shader_resource_view;//GPU���g�p���ăV�F�[�_�[�o�b�t�@�Ȃǂ̏��������
	D3D11_TEXTURE2D_DESC texture2d_desc; //2D �e�N�X�`���̃f�[�^�ۑ�


	const size_t max_vertices;
	std::vector<vertex> vertices;

public:

	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a // ��`�̃T�C�Y�i�X�N���[�����W�n�j
		, float angle
	);
	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a // ��`�̃T�C�Y�i�X�N���[�����W�n�j
		, float angle
		, float sx, float sy, float sw, float sh //�`�ʈʒu �T�C�Y
	);

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);

	void Begin(ID3D11DeviceContext* immediate_context);

	void End(ID3D11DeviceContext* immediate_context);
	/*�f�X�g���N�^�ł͂��ׂĂ� COM �I�u�W�F�N�g���������*/

	Sprite_Batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites); //wchar_t(��p�����p�X)
	~Sprite_Batch();

	//�摜�̕ύX

};

