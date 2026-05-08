// ===================================================
// collision.h 当たり判定
// 
// 制作者：				日付：2024
// ===================================================
#ifndef _COLLISION_H_
#define _COLLISION_H_

#include	"Main.h"

// ===================================================
// プロトタイプ宣言
// ===================================================
bool CheckBoxCollider(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB);	// バウンディングボックスの当たり判定
bool CheckCircleCollider(XMFLOAT3 PosA, XMFLOAT3 PosB, float rA, float rB);		// バウンディングサークルの当たり判定

bool CheckBoxColliderRightBottom(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB);
bool CheckBoxColliderRightTop(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB);
bool CheckBoxColliderLeftBottom(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB);
bool CheckBoxColliderLeftTop(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB);
bool CheckBoxColliderRight(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB);
bool CheckBoxColliderLeft(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB);




#endif

