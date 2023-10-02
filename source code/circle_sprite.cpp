#include <list>

#include "circle_sprite.h"
#include "misc.h"
#include <sstream>

#include "vertex.h"

namespace
{

	//vertex vertices[] //���_���̃Z�b�g�����ݒ�
	//{
	// { { -0.5, +0.5, 0 }, { 1, 1, 1, 1 } },
	// { { +0.5, +0.5, 0 }, { 1, 0, 0, 1 } },
	// { { -0.5, -0.5, 0 }, { 0, 1, 0, 1 } },
	// { { +0.5, -0.5, 0 }, { 0, 0, 1, 1 } },
	//};
	//vertex vertices_for_draw[] //���_���̃Z�b�g�����ݒ�
	//{
	//	vertices[0],
	//	vertices[1],
	//	vertices[2],
	//	vertices[2],
	//	vertices[1],
	//	vertices[3],
	//};

	//std::list<vertex*> vertexList_;


	//D3D11_BUFFER_DESC buffer_desc = {};


}
//�o�b�t�@�[�Ƃ̓f�[�^�̕ۑ��Ȃǂ��菕��������́i�ɏՑ́H�j
CircleSprite::CircleSprite(ID3D11Device* device)
{
	//�A���_�o�b�t�@�I�u�W�F�N�g�̐���
	HRESULT hr{ S_OK }; //�G���[���e�H ���݂͐���


	UINT vertex_count = NUM_CIRCLE_POINTS + 1;

	D3D11_BUFFER_DESC buffer_desc{};//�o�b�t�@�[
	buffer_desc.ByteWidth = sizeof(vertex) * vertex_count; //�o�b�t�@�[�̃T�C�Y
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC; //GPU�ɂ��ǂݎ��Ə�������
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//�o�b�t�@�[�𒸓_�o�b�t�@�[�Ƃ��Ďg���i���蓖�Ă�j
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU�g�p(���\�[�X)�̋�����
	buffer_desc.MiscFlags = 0; //���̑��̃��\�[�X�H
	buffer_desc.StructureByteStride = 0; //�o�b�t�@�[�̍\�����Ȃ��@�o�b�t�@�[�̍\�����T�C�Y
	D3D11_SUBRESOURCE_DATA subresource_data{};//�T�u���\�[�X
	subresource_data.pSysMem = nullptr; //�������f�[�^�̃|�C���^�[
	subresource_data.SysMemPitch = 0; //�e�N�X�`���� 1 �s�̐擪���玟�̍s�܂ł̋���
	subresource_data.SysMemSlicePitch = 0;//1 �̐[�x���x���̐擪���玟�̐[�����x���܂ł̋��� (�o�C�g�P��)
	hr = device->CreateBuffer(&buffer_desc, nullptr, &vertex_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	{
		//�C���f�b�N�X�o�b�t�@����
		D3D11_BUFFER_DESC buffer_desc{};//�o�b�t�@�[
		buffer_desc.ByteWidth = sizeof(UINT) * NUM_CIRCLE_POINTS * 3; //�o�b�t�@�[�̃T�C�Y
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC; //GPU�ɂ��ǂݎ��Ə�������
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;//�o�b�t�@�[�𒸓_�o�b�t�@�[�Ƃ��Ďg���i���蓖�Ă�j
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU�g�p(���\�[�X)�̋�����
		buffer_desc.MiscFlags = 0; //���̑��̃��\�[�X�H
		buffer_desc.StructureByteStride = 0; //�o�b�t�@�[�̍\�����Ȃ��@�o�b�t�@�[�̍\�����T�C�Y
		D3D11_SUBRESOURCE_DATA subresource_data{};//�T�u���\�[�X
		subresource_data.pSysMem = nullptr; //�������f�[�^�̃|�C���^�[
		subresource_data.SysMemPitch = 0; //�e�N�X�`���� 1 �s�̐擪���玟�̍s�܂ł̋���
		subresource_data.SysMemSlicePitch = 0;//1 �̐[�x���x���̐擪���玟�̐[�����x���܂ł̋��� (�o�C�g�P��)
		hr = device->CreateBuffer(&buffer_desc, nullptr, &index_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//���_�V�F�[�_�[�I�u�W�F�N�g�̐���


	const char* cso_name{ "sprite_vs.cso" };

	FILE* fp{};
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	//HRESULT hr{ S_OK }; //�G���[���e�H ���݂͐���

	hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr)); //�����Ă邩�ǂ���

	//���̓��C�A�E�g�I�u�W�F�N�g�̐���

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] //�摜�f�[�^
	{
	   { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	/*HRESULT */ hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc),
		cso_data.get(), cso_sz, &input_layout); 
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���

	/*const char**/ cso_name = { "sprite_ps.cso" };

	//FILE* fp{};
	fopen_s(&fp, cso_name, "rb"); //�f�[�^�m�F
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);//�t�@�C���̈ړ�
	cso_sz = { ftell(fp) }; //�ǂݍ��݈ʒu�̈ړ�
	fseek(fp, 0, SEEK_SET); //�t�@�C���̈ړ�

	/*std::unique_ptr<unsigned char[]>*/ cso_data = { std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp); //�\���̃|�C���^�[
	fclose(fp);//����

	/*HRESULT*/ hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, &pixel_shader); //�s�N�Z��(�_�̕���) �V�F�[�_�[���쐬
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//int centerX = 500
	//	, centerY = 500
	//	,
	//	radius = 500;

	// �~��`��
	//for (int i = 0; i <= 360; i++) {
	//	int angle = i * DirectX::XM_PI / 180.0;
	//	float x = centerX + radius * cos(angle);
	//	float y = centerY + radius * sin(angle);

	//	vertex v = { { x, y, 0 }, { 1, 1, 1, 1 } };

	//	vertexList_.emplace_back(&v);

	//}

}

CircleSprite::~CircleSprite()
{

}

namespace
{


	auto rotate = [](float& x, float& y, float cx, float cy, float angle)
	{
		x -= cx;
		y -= cy;

		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;


	};

}


void CircleSprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a // ��`�̃T�C�Y�i�X�N���[�����W�n�j
	, float angle
)
{

	//�@�X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

	//3�̓��e
	//�Arender �����o�֐��̈����idx, dy, dw, dh�j�����`�̊e���_�̈ʒu�i�X�N���[�����W�n�j���v�Z����
	// (x0, y0) *----* (x1, y1) 
	// |   /|
	// |  / |
	// | /  |
	// |/   |
	// (x2, y2) *----* (x3, y3) 

	// left-top
	float x0{ dx };
	float y0{ dy };
	// right-top
	float x1{ dx + dw };
	float y1{ dy };
	// left-bottom
	float x2{ dx };
	float y2{ dy + dh };
	// right-bottom
	float x3{ dx + dw };
	float y3{ dy + dh };



	//�B�X�N���[�����W�n���� NDC �ւ̍��W�ϊ��������Ȃ�
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	//�C�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	 
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	//GPU�̃������̏������݂̂��߂Ɉꎞ�I�ɋ��ۂ��
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource); 
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	vertex vertices[NUM_CIRCLE_POINTS + 1]; //���_�̕ϐ��z��

	vertices[NUM_CIRCLE_POINTS].position.x = 0;
	vertices[NUM_CIRCLE_POINTS].position.y = 0;
	vertices[NUM_CIRCLE_POINTS].position.z = 0;
	vertices[NUM_CIRCLE_POINTS].color = { 1,1,1,1 };

	for(UINT i = 0;i<NUM_CIRCLE_POINTS;++i) //�_�̏ꏊ�w��
	{
		//�~�̓_�iangle�̂悤�Ȃ��́j
		float theta = (float)i / (NUM_CIRCLE_POINTS);
		theta *= DirectX::XM_PI;
		;
		float x = cosf(theta);
		float y = sinf(theta);


		vertices[i].position = { x,y,0 };
		vertices[i].color = { 1,1,1,1 };
	}


	std::memcpy(mapped_subresource.pData, vertices, sizeof(vertices)); //�o�C�g���R�s�[


	//vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	//if (vertices != nullptr)
	//{
	//	vertices[0].position = { x0, y0 , 0 };
	//	vertices[1].position = { x1, y1 , 0 };
	//	vertices[2].position = { x2, y2 , 0 };
	//	vertices[3].position = { x2, y2 , 0 };
	//	vertices[4].position = { x1, y1 , 0 };
	//	vertices[5].position = { x3, y3 , 0 };




	//	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color =
	//		vertices[4].color = vertices[5].color
	//		= { 1.0f,1.0f,1.0f,1.0f };//�I�u�W�F�̐F


	//}



	////��]�̒��S����`�̒��S�_�ɂ����ꍇ
	//float cx = dx + dw * 0.5f;
	//float cy = dy + dh * 0.5f;
	//rotate(x0, y0, cx, cy, angle);
	//rotate(x1, y1, cx, cy, angle);
	//rotate(x2, y2, cx, cy, angle);
	//rotate(x3, y3, cx, cy, angle);

	immediate_context->Unmap(vertex_buffer, 0);//�}�b�v�̉����iGPU�̃������̕����j


	{//�C���f�b�N�X�o�b�t�@�ɏ�������


		HRESULT hr{ S_OK };
		D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
		//GPU�̃������̏������݂̂��߂Ɉꎞ�I�ɋ��ۂ��
		hr = immediate_context->Map(index_buffer,0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		UINT indices[NUM_CIRCLE_POINTS * 3]; //�~�̎O�p�`�̃f�[�^�ϐ�

		for (UINT i = 0; i < NUM_CIRCLE_POINTS; ++i)//�~�̎O�p�`�̏��ԍ쐬
		{
			UINT offset = i * 3; 
			indices[offset] = i + 1;
			if(i == NUM_CIRCLE_POINTS - 1)
			{
				indices[offset] = 0; //���S�_
			}
			indices[offset + 1] = i;//�O�p�`�̎���
			indices[offset + 2] = NUM_CIRCLE_POINTS;//�O�p�`�̒��S�_
		}

		std::memcpy(mapped_subresource.pData, indices, sizeof(indices));

		immediate_context->Unmap(index_buffer, 0);//�}�b�v�̉����iGPU�̃������̕����j
	}

	//2�̓��e  //�o�C���h�Ƃ͊֘A�Â��邷�����
	//�@���_�o�b�t�@�[�̃o�C���h�@
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset); //���_�o�b�t�@�[���̓A�Z���u���[
	//�C���f�b�N�X�o�b�t�@�Z�b�g
	immediate_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//�A�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
	//�O�p�`�̂Ȃ����
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST/*(���_��������ĂȂ���Όq����Ȃ�)*//*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP���_�𖳗���肻��킹����*/);
	//�B���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout);
	//�C�V�F�[�_�[�̃o�C���h
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);
	//�D�v���~�e�B�u�̕`��
	//immediate_context->Draw(NUM_CIRCLE_POINTS + 1, 0);
	immediate_context->DrawIndexed(NUM_CIRCLE_POINTS * 3, 0, 0);//�C���f�b�N�X�̕`��


}
