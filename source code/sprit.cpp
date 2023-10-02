#include <list>

#include "sprite.h"
#include "misc.h"
#include <sstream>


#include "vertex.h"
#include "../DirectXTK-jul2022/Inc/WICTextureLoader.h"

namespace
{


	vertex vertices[] //���_���̃Z�b�g�����ݒ�
	{
	 { { -0.5, +0.5, 0 }, { 1, 1, 1, 1 },{0,0}  },
	 { { +0.5, +0.5, 0 }, { 1, 0, 0, 1 },{1,0}  },
	 { { -0.5, -0.5, 0 }, { 0, 1, 0, 1 },{0,1}  },
	 { { +0.5, -0.5, 0 }, { 0, 0, 1, 1 },{1,1}  },
	};
	vertex vertices_for_draw[] //���_���̃Z�b�g�����ݒ�
	{
		vertices[0],
		vertices[1],
		vertices[2],
		vertices[2],
		vertices[1],
		vertices[3],
	};

	//�A���̓��C�A�E�g�v�f�̒ǉ�

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]//���\�[�X�f�[�^�`��
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, //�F�ł͂Ȃ��`�����l��
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


}


SpritrHLSI::SpritrHLSI(ID3D11Device* device, const wchar_t* filename)
{
	//�A���_�o�b�t�@�I�u�W�F�N�g�̐���
	HRESULT hr{ S_OK }; //�G���[���e�H ���݂͐���


	D3D11_BUFFER_DESC buffer_desc{};//�o�b�t�@�[
	buffer_desc.ByteWidth = sizeof(vertices_for_draw); //�o�b�t�@�[�̃T�C�Y
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC; //GPU�ɂ��ǂݎ��Ə�������
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//�o�b�t�@�[�𒸓_�o�b�t�@�[�Ƃ��Ďg���i���蓖�Ă�j
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU�g�p(���\�[�X)�̋�����
	buffer_desc.MiscFlags = 0; //���̑��̃��\�[�X�H
	buffer_desc.StructureByteStride = 0; //�o�b�t�@�[�̍\�����Ȃ��@�o�b�t�@�[�̍\�����T�C�Y

	D3D11_SUBRESOURCE_DATA subresource_data{};//�������f�[�^
	subresource_data.pSysMem = vertices_for_draw; //�������f�[�^�̃|�C���^�[
	subresource_data.SysMemPitch = 0; //�e�N�X�`���� 1 �s�̐擪���玟�̍s�܂ł̋���
	subresource_data.SysMemSlicePitch = 0;//1 �̐[�x���x���̐擪���玟�̐[�����x���܂ł̋��� (�o�C�g�P��)
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer); //�o�b�t�@�[ (���_�o�b�t�@�[�A�C���f�b�N�X �o�b�t�@�[�A�܂��̓V�F�[�_�[�萔�o�b�t�@�[) 
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//���_�V�F�[�_�[�I�u�W�F�N�g�̐���


	const char* cso_name{ "sprite_vs.cso" }; //�V�F�[�_�[�̌Ăэ��݂̂��߂̕�����

	FILE* fp{};
	fopen_s(&fp, cso_name, "rb"); //�t�@�C�����J��
	_ASSERT_EXPR_A(fp, "CSO File not found"); 

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) }; //�t�@�C���̍��W
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) }; //���j�[�N�|�C���^�[�����f�[�^��ǂݍ���
	fread(cso_data.get(), cso_sz, 1, fp); //�f�[�^��ǂݍ���
	fclose(fp); //����

	//HRESULT hr{ S_OK }; //�G���[���e�H ���݂͐���

	hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr)); //�����Ă邩�ǂ���

	//���̓��C�A�E�g�I�u�W�F�N�g�̐���


	/*HRESULT */ hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc),
		cso_data.get(), cso_sz, &input_layout); /*���_�V�F�[�_�[�̃f�[�^�����n���������
	���_�V�F�[�_�[���ς��΍ēx�n���Ȃ��Ƃ����Ȃ�*/
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐���

	/*�摜��ǂݍ��݃s�N�Z���ɕ����ĕ`�ʂ���*/

	/*const char**/ cso_name = { "sprite_ps.cso" }; //�V�F�[�_�[�̎g�p

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

	//�B�摜�t�@�C���̃��[�h�ƃV�F�[�_�[���\�[�X�r���[�I�u�W�F�N�g(ID3D11ShaderResourceView)�̐���

		 ID3D11Resource * resource{}; //���������̃o�b�t�@�[��e�N�X�`���[�ȂǁA�O���t�B�b�N�X�f�[�^���i�[���邽�߂̃I�u�W�F�N�g(GPU�p�H)
	 hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view); //
	 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	 resource->Release();

	//�C�e�N�X�`�����(D3D11_TEXTURE2D_DESC)�̎擾

	 ID3D11Texture2D* texture2d{};
	  hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d); //�f�[�^�̃L���X�g�i�ϊ��j
	  _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	//�e�N�X�`�� ���\�[�X�̃v���p�e�B���擾���܂��B
	  texture2d->GetDesc(&texture2d_desc); //���̎擾
	  texture2d->Release();

	
}

SpritrHLSI::~SpritrHLSI()
{
	vertex_shader->Release(); //���_�V�F�[�_�[�̃C���^�[�t�F�[�X�i���̃V�X�e���ƘA�����邽�߁H�j�Ǘ��V�X�e��
	 pixel_shader->Release(); //�s�N�Z���V�F�[�_�[�i�X�e�[�W�𐧌䂷��Ǘ��V�X�e���j
	input_layout->Release(); //���̓��C�A�E�g�i�A�b�Z���u���[�̑��荞�݂̒�`�j
	 vertex_buffer->Release(); //�o�b�t�@�[�i���_�V�F�[�_�[�̃f�[�^���i�[�j
	shader_resource_view->Release();//GPU���g�p���ăV�F�[�_�[�o�b�t�@�Ȃǂ̏��������

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

		//x *= 0.1f;
		//y *= 0.01f;
	};
}

void SpritrHLSI::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a // ��`�̃T�C�Y�i�X�N���[�����W�n�j
	, float angle
	
)
{


	//�@�e�N�X�`�����W�𒸓_�o�b�t�@�ɃZ�b�g����
		//������̓e�N�X�`���S�̂�\������
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


		//�@�e�N�X�`�����W�𒸓_�o�b�t�@�ɃZ�b�g����
		//������̓e�N�X�`���S�̂�\������
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	//GPU�̃������̏������݂̂��߂Ɉꎞ�I�ɋ��ۂ��
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) }; //�f�[�^���璸�_��������o��
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0 , 0 };
		vertices[1].position = { x1, y1 , 0 };
		vertices[2].position = { x2, y2 , 0 };
		vertices[3].position = { x3, y3 , 0 };
		//vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1, 1, 1, 1 };
		vertices[0].color = { 1,1,1,1 };
			vertices[1].color = { 0,0,1,1 };
			vertices[2].color = { 1,0,1,1 };
			vertices[3].color = { 1,1,0,1 };

		//vertices[0].texcoord = { 0, 0 };
		//vertices[1].texcoord = { 1, 0 };
		//vertices[2].texcoord = { 0, 1 };
		//vertices[3].texcoord = { 1, 1 };
			vertices[0].texcoord = { 0, 0 };
			vertices[1].texcoord = { 1, 0 };
			vertices[2].texcoord = { 0, 1 };
			vertices[3].texcoord = { 1, 1 };


	}


	immediate_context->Unmap(vertex_buffer, 0);//�}�b�v�̉����iGPU�̃������̕����j

	//�C�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����

	//HRESULT hr{ S_OK };
	//D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	////GPU�̃������̏������݂̂��߂Ɉꎞ�I�ɋ��ۂ��
	//hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	//if (vertices != nullptr)
	//{

	//	//��]�̒��S����`�̒��S�_�ɂ����ꍇ
	//	float cx = dx + dw * 0.5f;
	//	float cy = dy + dh * 0.5f;
	//	rotate(x0, y0, cx, cy, angle);
	//	rotate(x1, y1, cx, cy, angle);
	//	rotate(x2, y2, cx, cy, angle);
	//	rotate(x3, y3, cx, cy, angle);

	//	vertices[0].position = { x0, y0 , 0 };
	//	vertices[1].position = { x1, y1 , 0 };
	//	vertices[2].position = { x2, y2 , 0 };
	//	vertices[3].position = { x3, y3 , 0 };
	//	 vertices[4].position = { x1, y1 , 0 };
	//	 vertices[5].position = { x3, y3 , 0 };






	//}


	//immediate_context->Unmap(vertex_buffer, 0);//�}�b�v�̉����iGPU�̃������̕����j

	//2�̓��e
	//�@���_�o�b�t�@�[�̃o�C���h
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	//�A�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
	//�O�p�`�̂Ȃ����
	//immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST/*(���_��������ĂȂ���Όq����Ȃ�)*//*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP���_�𖳗���肻��킹����*/);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//�B���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout);
	//�C�V�F�[�_�[�̃o�C���h
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	//�A�V�F�[�_�[ ���\�[�X�̃o�C���h
	immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

	//�D�v���~�e�B�u�̕`��
	immediate_context->Draw(4, 0);
}


void SpritrHLSI::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a // ��`�̃T�C�Y�i�X�N���[�����W�n�j
	, float angle
	, float sx, float sy, float sw, float sh
)
{

	//�@�e�N�X�`�����W�𒸓_�o�b�t�@�ɃZ�b�g����
		//������̓e�N�X�`���S�̂�\������
	//�@�X�N���[���i�r���[�|�[�g�j�̃T�C�Y���擾����
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);

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


	//�@�e�N�X�`�����W�𒸓_�o�b�t�@�ɃZ�b�g����
	//������̓e�N�X�`���S�̂�\������
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	//GPU�̃������̏������݂̂��߂Ɉꎞ�I�ɋ��ۂ��
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	float u0{ sx / texture2d_desc.Width };
	float v0{ sy / texture2d_desc.Height };
	float u1{ (sx + sw) / texture2d_desc.Width };
	float v1{ (sy + sh) / texture2d_desc.Height };


	vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) }; //�f�[�^���璸�_��������o��
	if (vertices != nullptr)
	{
		//��]�̒��S����`�̒��S�_�ɂ����ꍇ
		float cx = dx + dw * 0.5f;
		float cy = dy + dh * 0.5f;
		rotate(x0, y0, cx, cy, angle);
		rotate(x1, y1, cx, cy, angle);
		rotate(x2, y2, cx, cy, angle);
		rotate(x3, y3, cx, cy, angle);

		vertices[0].position = { x0, y0 , 0 };
		vertices[1].position = { x1, y1 , 0 };
		vertices[2].position = { x2, y2 , 0 };
		vertices[3].position = { x3, y3 , 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1, 1, 1, a };
		//vertices[0].color = { 1,1,1,0 };
		//vertices[1].color = { 0,0,1,0 };
		//vertices[2].color = { 1,0,1,0 };
		//vertices[3].color = { 1,1,0,1 };

		vertices[0].texcoord = { u0, v0 };
		vertices[1].texcoord = { u1, v0 };
		vertices[2].texcoord = { u0, v1 };
		vertices[3].texcoord = { u1, v1 };
	}
	//�C�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����

	immediate_context->Unmap(vertex_buffer, 0);//�}�b�v�̉����iGPU�̃������̕����j










//2�̓��e
//�@���_�o�b�t�@�[�̃o�C���h
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	//�A�v���~�e�B�u�^�C�v����уf�[�^�̏����Ɋւ�����̃o�C���h
	//���̂Ȃ����
	//immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST/*(���_��������ĂȂ���Όq����Ȃ�)*//*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP���_�𖳗���肻��킹����*/);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//�B���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	immediate_context->IASetInputLayout(input_layout);
	//�C�V�F�[�_�[�̃o�C���h
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	//�A�V�F�[�_�[ ���\�[�X�̃o�C���h
	immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

	//ID3D11RasterizerState* rasterizer_state{};

	//immediate_context->RSSetState(rasterizer_state);


	//�D�v���~�e�B�u�̕`��
	immediate_context->Draw(4, 0);

	//rasterizer_state->Release();
}

//�����`�ʏ���
 void SpritrHLSI::textout(ID3D11DeviceContext* immediate_context, std::string s,
	 float x, float y, float w, float h, float r, float g, float b, float a)
{
	float sw = static_cast<float>(texture2d_desc.Width / 16);
	float sh = static_cast<float>(texture2d_desc.Height / 16);
	float carriage = 0;
	for (const char c : s)
	{
			render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0,
			sw * (c & 0x0F), sh * (c >> 4), sw, sh);
			carriage += w;
	}
}
