/*---------------------------------------
　頂点管理[Fade.h]
                                  Auter :
                                  Date  :
---------------------------------------*/
#pragma once

#include "main.h"
#include "renderer.h"

/*---------------------------------------
　マクロ定義
---------------------------------------*/

/*---------------------------------------
　プロトタイプ宣言
---------------------------------------*/
void InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);
void FadeOut(void);
bool IsFadeOutFinish(void);