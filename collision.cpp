// ===================================================
// collision.cpp 当たり判定
// 
// 制作者：				日付：2024
// ===================================================
#include "main.h"
#include "collision.h"

// ===================================================
// バウンディングボックスの当たり判定
// 
// 引数:
// 	矩形Ａの中心座標
// 	矩形Ｂの中心座標
// 	矩形Ａのサイズ
// 	矩形Ｂのサイズ
//
// 戻り値
//  true：当たっている
// 	false：当たっていない
// ===================================================
bool CheckBoxCollider(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB)
{
	float ATop =	PosA.y - SizeA.y / 2;	// Aの上端
	float ABottom =	PosA.y + SizeA.y / 2;	// Aの下端
	float ARight =	PosA.x + SizeA.x / 2;	// Aの右端
	float ALeft =	PosA.x - SizeA.x / 2;	// Aの左端

	float BTop =	PosB.y - SizeB.y / 2;	// Bの上端
	float BBottom =	PosB.y + SizeB.y / 2;	// Bの下端
	float BRight =	PosB.x + SizeB.x / 2;	// Bの右端
	float BLeft =	PosB.x - SizeB.x / 2;	// Bの左端

	if ((ARight >= BLeft) &&		// Aの右端 > Bの左端
		(ALeft <= BRight) &&		// Aの左端 < Bの右端
		(ABottom >= BTop) &&		// Aの下端 > Bの上端
		(ATop <= BBottom))			// Aの上端 < Bの下端
	{
		// 当たっている
		return true;
	}

	// 当たっていない
	return false;
}

// ===================================================
// バウンディングサークルの当たり判定
// 
// 引数:
// 	円１の中心座標
// 	円２の中心座標
// 	円１の半径
// 	円２の半径
//
// 戻り値
//  true：当たっている
// 	false：当たっていない
// ===================================================
bool CheckCircleCollider(XMFLOAT3 PosA, XMFLOAT3 PosB, float rA, float rB)
{
	// (円Aの中心座標X - 円Bの中心座標X)の2乗 + (円Aの中心座標Y - 円Bの中心座標Y)の2乗
	float distance = (PosA.x - PosB.x) * (PosA.x - PosB.x) + (PosA.y - PosB.y) * (PosA.y - PosB.y);
	
	// (円1の半径+円2の半径)の2乗
	float rSum = (rA + rB) * (rA + rB);

	// 2つの円の距離が半径の合計を下回った時
	if (distance <= rSum)
	{
		// 当たっている
		return true;
	}

	// 当たっていない
	return false;
}

//Aの左上の点がBの矩形内にめり込んでいるか
bool CheckBoxColliderLeftTop(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB)
{
	//キャラクター
	float ATop = PosA.y - SizeA.y / 2;	// Aの上端
	float ALeft = PosA.x - SizeA.x / 2;	// Aの左端

	//背景
	float BTop = PosB.y - SizeB.y / 2;	// Bの上端
	float BBottom = PosB.y + SizeB.y / 2;	// Bの下端
	float BRight = PosB.x + SizeB.x / 2;	// Bの右端
	float BLeft = PosB.x - SizeB.x / 2;	// Bの左端

	//点(ALeft,ATop)が矩形Bの左右の範囲内か？
	if ((ALeft >= BLeft) && (ALeft <= BRight))
	{
		//上
		if ((ATop <= BBottom) && (ATop >= BTop))
		{
			//	左上めり込んでいる
			return	true;
		}
	}
	return	false;

}

//Aの右上の点がBの矩形内にめり込んでいるか
bool CheckBoxColliderRightTop(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB)
{
	//キャラクター
	float ATop = PosA.y - SizeA.y / 2;	// Aの上端
//	float ABottom = PosA.y + SizeA.y / 2;	// Aの下端
	float ARight = PosA.x + SizeA.x / 2;	// Aの右端
//	float ALeft = PosA.x - SizeA.x / 2;	// Aの左端

	//背景
	float BTop = PosB.y - SizeB.y / 2;	// Bの上端
	float BBottom = PosB.y + SizeB.y / 2;	// Bの下端
	float BRight = PosB.x + SizeB.x / 2;	// Bの右端
	float BLeft = PosB.x - SizeB.x / 2;	// Bの左端

	//点(ALeft,ATop)が矩形Bの左右の範囲内か？
	if ((ARight >= BLeft) && (ARight <= BRight))
	{
		//上
		if ((ATop <= BBottom) && (ATop >= BTop))
		{
			//	右上めり込んでいる
			return	true;
		}
	}
	return	false;

}
//Aの左下の点がBの矩形内にめり込んでいるか
bool CheckBoxColliderLeftBottom(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB)
{
	//キャラクター
	//float ATop = PosA.y - SizeA.y / 2;	// Aの上端
	float ABottom = PosA.y + SizeA.y / 2;	// Aの下端
	//float ARight = PosA.x + SizeA.x / 2;	// Aの右端
	float ALeft = PosA.x - SizeA.x / 2;	// Aの左端

		//背景
	float BTop = PosB.y - SizeB.y / 2;	// Bの上端
	float BBottom = PosB.y + SizeB.y / 2;	// Bの下端
	float BRight = PosB.x + SizeB.x / 2;	// Bの右端
	float BLeft = PosB.x - SizeB.x / 2;	// Bの左端

	//点(ALeft,ATop)が矩形Bの左右の範囲内か？
	if ((ALeft >= BLeft) && (ALeft <= BRight))
	{
		//上
		if ((ABottom <= BBottom) && (ABottom >= BTop))
		{
			//	左上めり込んでいる
			return	true;
		}
	}
	return	false;

}
//Aの右下の点がBの矩形内にめり込んでいるか
bool CheckBoxColliderRightBottom(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB)
{
	//キャラクター
	//float ATop = PosA.y - SizeA.y / 2;	// Aの上端
	float ABottom = PosA.y + SizeA.y / 2;	// Aの下端
	float ARight = PosA.x + SizeA.x / 2;	// Aの右端
	//float ALeft = PosA.x - SizeA.x / 2;	// Aの左端

	//背景
	float BTop = PosB.y - SizeB.y / 2;	// Bの上端
	float BBottom = PosB.y + SizeB.y / 2;	// Bの下端
	float BRight = PosB.x + SizeB.x / 2;	// Bの右端
	float BLeft = PosB.x - SizeB.x / 2;	// Bの左端

	//点(ALeft,ATop)が矩形Bの左右の範囲内か？
	if ((ARight >= BLeft) && (ARight <= BRight))
	{
		//上
		if ((ABottom <= BBottom) && (ABottom >= BTop))
		{
			//	左上めり込んでいる
			return	true;
		}
	}
	return	false;
}

bool CheckBoxColliderRight(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB)
{
	//キャラクター
	//float ATop = PosA.y - SizeA.y / 2;	// Aの上端
	float ABottom = PosA.y;	// Aの下端
	float ARight = PosA.x + SizeA.x / 2;	// Aの右端
	//float ALeft = PosA.x - SizeA.x / 2;	// Aの左端

	//背景
	float BTop = PosB.y - SizeB.y / 2;	// Bの上端
	float BBottom = PosB.y + SizeB.y / 2;	// Bの下端
	float BRight = PosB.x + SizeB.x / 2;	// Bの右端
	float BLeft = PosB.x - SizeB.x / 2;	// Bの左端

	//点(ALeft,ATop)が矩形Bの左右の範囲内か？
	if ((ARight >= BLeft) && (ARight <= BRight))
	{
		//上
		if ((ABottom <= BBottom) && (ABottom >= BTop))
		{
			//	左上めり込んでいる
			return	true;
		}
	}
	return	false;
}
bool CheckBoxColliderLeft(XMFLOAT3 PosA, XMFLOAT3 PosB, XMFLOAT3 SizeA, XMFLOAT3 SizeB)
{
	//キャラクター
	//float ATop = PosA.y - SizeA.y / 2;	// Aの上端
	float ABottom = PosA.y;	// Aの下端
	//float ARight = PosA.x + SizeA.x / 2;	// Aの右端
	float ALeft = PosA.x - SizeA.x / 2;	// Aの左端

	//背景
	float BTop = PosB.y - SizeB.y / 2;	// Bの上端
	float BBottom = PosB.y + SizeB.y / 2;	// Bの下端
	float BRight = PosB.x + SizeB.x / 2;	// Bの右端
	float BLeft = PosB.x - SizeB.x / 2;	// Bの左端

	//点(ALeft,ATop)が矩形Bの左右の範囲内か？
	if ((ALeft >= BLeft) && (ALeft <= BRight))
	{
		//上
		if ((ABottom <= BBottom) && (ABottom >= BTop))
		{
			//	左上めり込んでいる
			return	true;
		}
	}
	return	false;
}

