/*---------------------------------------
　頂点管理[block.h]
                                  Auter :
                                  Date  :
---------------------------------------*/
#pragma once

#include "main.h"
#include "renderer.h"

/*---------------------------------------
　マクロ定義
---------------------------------------*/
struct BLOCK
{
    bool Enable;
    bool Erase;
    int Type;
    bool Flashing;  // 点滅中かどうか
    int FlashCount; // 点滅のカウント
};

/*---------------------------------------
　プロトタイプ宣言
---------------------------------------*/
void InitBlock(void);
void UninitBlock(void);
void UpdateBlock(void);
void DrawBlock(void);
void SetBlock(int x, int y, int Type);
BLOCK GetBlock(int x, int y);
void EraseBlock();
void StackBlock();