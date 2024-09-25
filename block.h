/*---------------------------------------
�@���_�Ǘ�[block.h]
                                  Auter :
                                  Date  :
---------------------------------------*/
#pragma once

#include "main.h"
#include "renderer.h"

/*---------------------------------------
�@�}�N����`
---------------------------------------*/
struct BLOCK
{
    bool Enable;
    bool Erase;
    int Type;
    bool Flashing;  // �_�Œ����ǂ���
    int FlashCount; // �_�ł̃J�E���g
};

/*---------------------------------------
�@�v���g�^�C�v�錾
---------------------------------------*/
void InitBlock(void);
void UninitBlock(void);
void UpdateBlock(void);
void DrawBlock(void);
void SetBlock(int x, int y, int Type);
BLOCK GetBlock(int x, int y);
void EraseBlock();
void StackBlock();