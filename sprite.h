/*---------------------------------------
�@���_�Ǘ�[sprite.h]
                                  Auter :
                                  Date  :
---------------------------------------*/
#pragma once

#include "main.h"
#include "renderer.h"

/*---------------------------------------
�@�}�N����`
---------------------------------------*/

/*---------------------------------------
�@�v���g�^�C�v�錾
---------------------------------------*/
void InitSprite(void);
void UninitSprite(void);
void DrawSprite(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale, float Alpha = 1.0f);
void DrawSpriteAnim(XMFLOAT2 Position, float Rotation, XMFLOAT2 Scale, int AnimCols, int AnimRows, int AnimPattern);