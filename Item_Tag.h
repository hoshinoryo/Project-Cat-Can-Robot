/*==============================================================================

   アイテム種類の定義 [Item_Tag.h]
                                                         Author : Gu Anyi
                                                         Date   : 2026/07/10

--------------------------------------------------------------------------------

==============================================================================*/
#pragma once

enum class ItemTag
{
	FISH,
	BOMB,
	ITEM_TAG_MAX
};

inline int GetItemScore(ItemTag tag)
{
	switch (tag)
	{
	case ItemTag::FISH:
		return 1;

	case ItemTag::BOMB:
		return -1;

	default:
		return 0;
	}
}

inline const char* GetItemAssetPath(ItemTag tag)
{
	switch (tag)
	{
	case ItemTag::FISH:
		return "Asset/Environment/Item/Item_Fish.fbx";

	case ItemTag::BOMB:
		return "Asset/Environment/Item/Item_Bomb.fbx";

	default:
		return "";
	}
}
