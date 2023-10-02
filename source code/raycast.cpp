// RAYCAST
#include "raycast.h"
#include "skinned_mesh.h"

using namespace DirectX;

// The function returns the index of the triangle if it intersects, or -1 if it does not.この関数は、三角形が交差する場合はそのインデックスを返し、
// 交差しない場合は -1 を返します。
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
	// 光線と三角形の交差
	// この配布資料では、光線と三角形の交差を計算し、
	// その交差の重心座標を計算するために必要な手順を検討します。
	//まず、そのような交差の幾何学形状を検討します。 
	// ここで、原点 P と方向 D を持つ光線は、交点 Q で頂点 A、B、C で定義される三角形と
	// 交差します。三角形 ABC を図的に囲む正方形の領域は、
	// 三角形の支持面を表します。 、つまり、三角形が置かれる平面です。 
	// 光線と三角形の交差を実行するには、次の 2 つの手順を実行する必要があります。 
	// 1. 交点 Q を決定します。 
	// 2. Q が三角形 ABC の内側にあるかどうかを決定します。
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
		// We・ｽfll handle these steps in reverse order.
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
//	const float ray_length_limit{ distance };//距離の最大の長さ
//	float closest_distance{ FLT_MAX };
//	XMVECTOR X{}; // closest cross point最も近いクロスポイント
//	const XMVECTOR P{ XMVectorSet(ray_position.x, ray_position.y, ray_position.z, 1) };//始点
//	const XMVECTOR D{ XMVector3Normalize(XMVectorSet(ray_direction.x, ray_direction.y, ray_direction.z, 0)) };//距離の方向
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
//		const XMVECTOR A{ XMVectorSet(//ゲ―プロ３と同じ
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
//		// We・ｽfll handle these steps in reverse order.
//
//		//Q の決定
//		// 交点 Q を決定するには、さらに 2 つの手順を実行する必要があります。
//		// 1. 三角形 ABC の支持面の方程式を決定します。
//		// 2. 光線をサポート平面と交差させます。
//		// これらのステップを逆の順序で処理します。
//		XMVECTOR Q;
//
//		// 1. Determine the equation of the supporting plane for triangle ABC.
//		// 1. 三角形 ABC の支持面の方程式を求めます。
//		const XMVECTOR N{ XMVector3Normalize(XMVector3Cross(XMVectorSubtract(B, A), XMVectorSubtract(C, A))) };
//		const float d{ XMVectorGetByIndex(XMVector3Dot(N, A), 0) };
//
//		// 2. Intersect the ray with the supporting plane.
//		// 2. 光線をサポート平面と交差させます。
//		const float denominator{ XMVectorGetByIndex(XMVector3Dot(N, D), 0) };
//		if (denominator < 0) // Note that if N.D = 0 , then D is parallel to the plane and the ray does not intersect the plane.
//			// N.D = 0 の場合、D は平面に平行であり、光線は平面と交差しないことに注意してください。
//		{
//			const float numerator{ d - XMVectorGetByIndex(XMVector3Dot(N, P), 0) };
//			const float t{ numerator / denominator };
//
//			if (t > 0 && t < ray_length_limit) // Forward and Length limit of Ray光線の前方および長さの制限
//			{
//				Q = XMVectorAdd(P, XMVectorScale(D, t));
//
//				// Triangle inside - outside testing 三角形の内側と外側のテスト
//
//				// CHRISTER ERICSON. "REAL-TIME COLLISION DETECTION" pp.204
//				// Test if point Q lies inside the counterclockwise 3D triangle ABC
//				//(クリステル・エリクソン。 "リアルタイム衝突検出" pp.204
//				//点 Q が反時計回りの 3D 三角形 ABC の内側にあるかどうかをテストします)
//
//				// Translate point and triangle so that point lies at origin
//				// 点と三角形を移動して、その点が原点にくるようにします
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
//				//それ以外の場合、Q は三角形の中(または上) になければなりません
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
//		//ローカル空間からワールド空間へ変換
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
