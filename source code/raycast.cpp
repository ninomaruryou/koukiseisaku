// RAYCAST
#include "raycast.h"
#include "skinned_mesh.h"

using namespace DirectX;

// The function returns the index of the triangle if it intersects, or -1 if it does not.���̊֐��́A�O�p�`����������ꍇ�͂��̃C���f�b�N�X��Ԃ��A
// �������Ȃ��ꍇ�� -1 ��Ԃ��܂��B
int intersect_ray_triangles
(
	const float* positions, // vertices
	const uint32_t offset, // bytes
	const uint32_t stride, // bytes
	const uint32_t* indices,
	const size_t index_count,
	const XMFLOAT4& ray_position,
	const XMFLOAT4& ray_direction,
	XMFLOAT4& intersection,
	float& distance,
	const bool RHS
)
{
	const bool CCW{ RHS }; // Counterclockwise 3D triangle ABC
	const int C0{ 0 };
	const int C1{ CCW ? 1 : 2 };
	const int C2{ CCW ? 2 : 1 };

	// Ray-triangle intersection 
	// In this handout, we explore the steps needed to compute the intersection of a ray with a triangle, and then to
	// compute the barycentric coordinates of that intersection.First, we consider the geometry of such an intersection :
	//
	// where a ray with origin P and direction D intersects a triangle defined by its vertices, A, B, and C at intersection
	// point Q.The square region diagrammatically surrounding triangle ABC represents the supporting plane of the
	// triangle, i.e., the plane that the triangle lies on.
	//
	// To perform ray - triangle intersection, we must perform two steps :
	//
	//  1. Determine the point of intersection, Q.
	//  2. Determine if Q lies inside of triangle ABC.
	// �����ƎO�p�`�̌���
	// ���̔z�z�����ł́A�����ƎO�p�`�̌������v�Z���A
	// ���̌����̏d�S���W���v�Z���邽�߂ɕK�v�Ȏ菇���������܂��B
	//�܂��A���̂悤�Ȍ����̊􉽊w�`����������܂��B 
	// �����ŁA���_ P �ƕ��� D ���������́A��_ Q �Œ��_ A�AB�AC �Œ�`�����O�p�`��
	// �������܂��B�O�p�` ABC ��}�I�Ɉ͂ސ����`�̗̈�́A
	// �O�p�`�̎x���ʂ�\���܂��B �A�܂�A�O�p�`���u����镽�ʂł��B 
	// �����ƎO�p�`�̌��������s����ɂ́A���� 2 �̎菇�����s����K�v������܂��B 
	// 1. ��_ Q �����肵�܂��B 
	// 2. Q ���O�p�` ABC �̓����ɂ��邩�ǂ��������肵�܂��B
	//

#if 1
	int intersection_count{ 0 };
	int intersected_triangle_index{ -1 };

	const float ray_length_limit{ distance };
	float closest_distance{ FLT_MAX };

	XMVECTOR X{}; // closest cross point
	const XMVECTOR P{ XMVectorSet(ray_position.x, ray_position.y, ray_position.z, 1) };
	const XMVECTOR D{ XMVector3Normalize(XMVectorSet(ray_direction.x, ray_direction.y, ray_direction.z, 0)) };

	using byte = uint8_t;
	const byte* p{ reinterpret_cast<const byte*>(positions) + offset };
	const size_t triangle_count{ index_count / 3 };
	for (size_t triangle_index = 0; triangle_index < triangle_count; triangle_index++)
	{
		const XMVECTOR A{ XMVectorSet(
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C0] * stride))[0],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C0] * stride))[1],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C0] * stride))[2],
			1.0f
		) };
		const XMVECTOR B{ XMVectorSet(
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C1] * stride))[0],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C1] * stride))[1],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C1] * stride))[2],
			1.0f
		) };
		const XMVECTOR C{ XMVectorSet(
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C2] * stride))[0],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C2] * stride))[1],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C2] * stride))[2],
			1.0f
		) };

		// Determining Q
		// Determining the point of intersection Q in turn requires us to take two more steps :
		//  1. Determine the equation of the supporting plane for triangle ABC.
		//  2. Intersect the ray with the supporting plane.
		// We�E�fll handle these steps in reverse order.
		XMVECTOR Q;

		// 1. Determine the equation of the supporting plane for triangle ABC.
		const XMVECTOR N{ XMVector3Normalize(XMVector3Cross(XMVectorSubtract(B, A), XMVectorSubtract(C, A))) };
		const float d{ XMVectorGetByIndex(XMVector3Dot(N, A), 0) };

		// 2. Intersect the ray with the supporting plane.
		const float denominator{ XMVectorGetByIndex(XMVector3Dot(N, D), 0) };
		if (denominator < 0) // Note that if N.D = 0 , then D is parallel to the plane and the ray does not intersect the plane.
		{
			const float numerator{ d - XMVectorGetByIndex(XMVector3Dot(N, P), 0) };
			const float t{ numerator / denominator };

			if (t > 0 && t < ray_length_limit) // Forward and Length limit of Ray
			{
				Q = XMVectorAdd(P, XMVectorScale(D, t));

				// Triangle inside - outside testing

				// CHRISTER ERICSON. "REAL-TIME COLLISION DETECTION" pp.204
				// Test if point Q lies inside the counterclockwise 3D triangle ABC

				// Translate point and triangle so that point lies at origin

				const XMVECTOR QA{ XMVectorSubtract(A, Q) };
				const XMVECTOR QB{ XMVectorSubtract(B, Q) };
				const XMVECTOR QC{ XMVectorSubtract(C, Q) };

				XMVECTOR U{ XMVector3Cross(QB, QC) };
				XMVECTOR V{ XMVector3Cross(QC, QA) };
				if (XMVectorGetByIndex(XMVector3Dot(U, V), 0) < 0)
				{
					continue;
				}

				XMVECTOR W{ XMVector3Cross(QA, QB) };
				if (XMVectorGetByIndex(XMVector3Dot(U, W), 0) < 0)
				{
					continue;
				}
				if (XMVectorGetByIndex(XMVector3Dot(V, W), 0) < 0)
				{
					continue;
				}

				// Otherwise Q must be in (or on) the triangle
				if (t < closest_distance)
				{
					closest_distance = t;
					intersected_triangle_index = static_cast<int>(triangle_index);
					X = Q;
				}
				intersection_count++;
			}
		}
	}
	if (intersection_count > 0)
	{
		XMStoreFloat4(&intersection, X);
		distance = closest_distance;
	}
	return intersected_triangle_index;
#endif

}

//int intersect_ray_triangles(const float* positions, const uint32_t offset, const uint32_t stride, const uint32_t* indices, const size_t index_count, const DirectX::XMFLOAT4& ray_position, const DirectX::XMFLOAT4& ray_direction, DirectX::XMFLOAT4& intersection, float& distance, Skinned_Mesh::HitResult& result, const DirectX::XMFLOAT4X4& world_transform, const bool RHS)
//{
//	const bool CCW{ RHS }; // Counterclockwise 3D triangle ABC
//	const int C0{ 0 };
//	const int C1{ CCW ? 1 : 2 };
//	const int C2{ CCW ? 2 : 1 };
//
//# if 1
//
//	int intersection_count{ 0 };
//	int intersected_triangle_index{ -1 };
//
//	const float ray_length_limit{ distance };//�����̍ő�̒���
//	float closest_distance{ FLT_MAX };
//	XMVECTOR X{}; // closest cross point�ł��߂��N���X�|�C���g
//	const XMVECTOR P{ XMVectorSet(ray_position.x, ray_position.y, ray_position.z, 1) };//�n�_
//	const XMVECTOR D{ XMVector3Normalize(XMVectorSet(ray_direction.x, ray_direction.y, ray_direction.z, 0)) };//�����̕���
//
//	DirectX::XMVECTOR HitPosition;
//	DirectX::XMVECTOR HitNormal;
//
//	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat4(&ray_position);
//	DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&world_transform);
//	DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
//
//	using byte = uint8_t;
//	const byte* p{ reinterpret_cast<const byte*>(positions) + offset };
//	const size_t triangle_count{ index_count / 3 };
//	for (size_t triangle_index = 0; triangle_index < triangle_count; triangle_index++)
//	{
//
//
//		//DirectX::XMVECTOR P = DirectX::XMVectorAdd(S, DirectX::XMVectorScale(V, x));
//
//		//HitPosition = P;
//		//HitNormal = N;
//
//		const XMVECTOR A{ XMVectorSet(//�Q�\�v���R�Ɠ���
//			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C0] * stride))[0],
//			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C0] * stride))[1],
//			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C0] * stride))[2],
//			1.0f
//		) };
//		const XMVECTOR B{ XMVectorSet(
//			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C1] * stride))[0],
//			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C1] * stride))[1],
//			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C1] * stride))[2],
//			1.0f
//		) };
//		const XMVECTOR C{ XMVectorSet(
//			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C2] * stride))[0],
//			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C2] * stride))[1],
//			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C2] * stride))[2],
//			1.0f
//		) };
//
//		// Determining Q
//		// Determining the point of intersection Q in turn requires us to take two more steps :
//		//  1. Determine the equation of the supporting plane for triangle ABC.
//		//  2. Intersect the ray with the supporting plane.
//		// We�E�fll handle these steps in reverse order.
//
//		//Q �̌���
//		// ��_ Q �����肷��ɂ́A����� 2 �̎菇�����s����K�v������܂��B
//		// 1. �O�p�` ABC �̎x���ʂ̕����������肵�܂��B
//		// 2. �������T�|�[�g���ʂƌ��������܂��B
//		// �����̃X�e�b�v���t�̏����ŏ������܂��B
//		XMVECTOR Q;
//
//		// 1. Determine the equation of the supporting plane for triangle ABC.
//		// 1. �O�p�` ABC �̎x���ʂ̕����������߂܂��B
//		const XMVECTOR N{ XMVector3Normalize(XMVector3Cross(XMVectorSubtract(B, A), XMVectorSubtract(C, A))) };
//		const float d{ XMVectorGetByIndex(XMVector3Dot(N, A), 0) };
//
//		// 2. Intersect the ray with the supporting plane.
//		// 2. �������T�|�[�g���ʂƌ��������܂��B
//		const float denominator{ XMVectorGetByIndex(XMVector3Dot(N, D), 0) };
//		if (denominator < 0) // Note that if N.D = 0 , then D is parallel to the plane and the ray does not intersect the plane.
//			// N.D = 0 �̏ꍇ�AD �͕��ʂɕ��s�ł���A�����͕��ʂƌ������Ȃ����Ƃɒ��ӂ��Ă��������B
//		{
//			const float numerator{ d - XMVectorGetByIndex(XMVector3Dot(N, P), 0) };
//			const float t{ numerator / denominator };
//
//			if (t > 0 && t < ray_length_limit) // Forward and Length limit of Ray�����̑O������ђ����̐���
//			{
//				Q = XMVectorAdd(P, XMVectorScale(D, t));
//
//				// Triangle inside - outside testing �O�p�`�̓����ƊO���̃e�X�g
//
//				// CHRISTER ERICSON. "REAL-TIME COLLISION DETECTION" pp.204
//				// Test if point Q lies inside the counterclockwise 3D triangle ABC
//				//(�N���X�e���E�G���N�\���B "���A���^�C���Փˌ��o" pp.204
//				//�_ Q �������v���� 3D �O�p�` ABC �̓����ɂ��邩�ǂ������e�X�g���܂�)
//
//				// Translate point and triangle so that point lies at origin
//				// �_�ƎO�p�`���ړ����āA���̓_�����_�ɂ���悤�ɂ��܂�
//				const XMVECTOR QA{ XMVectorSubtract(A, Q) };
//				const XMVECTOR QB{ XMVectorSubtract(B, Q) };
//				const XMVECTOR QC{ XMVectorSubtract(C, Q) };
//
//				XMVECTOR U{ XMVector3Cross(QB, QC) };
//				XMVECTOR V{ XMVector3Cross(QC, QA) };
//				if (XMVectorGetByIndex(XMVector3Dot(U, V), 0) < 0)
//				{
//					continue;
//				}
//
//				XMVECTOR W{ XMVector3Cross(QA, QB) };
//				if (XMVectorGetByIndex(XMVector3Dot(U, W), 0) < 0)
//				{
//					continue;
//				}
//				if (XMVectorGetByIndex(XMVector3Dot(V, W), 0) < 0)
//				{
//					continue;
//				}
//
//
//				// Otherwise Q must be in (or on) the triangle
//				//����ȊO�̏ꍇ�AQ �͎O�p�`�̒�(�܂��͏�) �ɂȂ���΂Ȃ�܂���
//				if (t < closest_distance)
//				{
//					closest_distance = t;
//					intersected_triangle_index = static_cast<int>(triangle_index);
//					X = Q;
//
//
//					HitPosition = Q;
//					HitNormal = N;
//
//				}
//				intersection_count++;
//			}
//		}
//	}
//	if (intersection_count > 0)
//	{
//		//���[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
//		DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
//
//		DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
//		DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
//		DirectX::XMStoreFloat(&distance, WorldCrossLength);
//
//		XMStoreFloat4(&intersection, X);
//		distance = closest_distance;
//
//		DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
//
//		result.distance = distance;
//		//result.materialIndex = materialIndex;
//		DirectX::XMStoreFloat3(&result.position, WorldPosition);
//		DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
//
//		//if (result.distance < distance)
//		//	intersected_triangle_index = -1;
//	}
//	return intersected_triangle_index;
//#endif
//
//}
