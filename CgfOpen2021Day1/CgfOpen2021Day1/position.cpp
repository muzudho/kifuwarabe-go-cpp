#include "common.h"
#include "position.h"
#include <iostream>
#include <iomanip>

/// <summary>
/// 盤の描画
/// </summary>
void Position::PrintBoard()
{
    // 筋
    int x;

    // 段
    int y;

    // 石
    const char *str[4] = {".", "X", "O", "#"};

    // 着手点
    int played_z = 0;

    // 手番の色
    int color = 0;

    // 1手前の着手
    if (moves > 0)
    {
        played_z = record[moves - 1];
        color = Board[played_z];
    }

    // 筋符号のリスト
    std::cerr << "   ";
    for (x = 0; x < kBoardSize; x++) {
        std::cerr << (char)('A' + x + (x > 7));
    }
    std::cerr << std::endl;

    // テーブル
    for (y = 0; y < kBoardSize; y++)
    {
        // 行見出し
        std::cerr << std::setw(2) << (kBoardSize - y) << " ";

        for (x = 0; x < kBoardSize; x++)
        {
            std::cerr << str[Board[GetZ(x + 1, y + 1)]];
        }

        if (y == 4) {
            std::cerr << "  ko_z=" << GetCharZ(ko_z) << ",moves=" << moves;
        }
        else if (y == 7) {
            std::cerr << "  play_z=" << GetCharZ(played_z) << ", color=" << color;
        }

        std::cerr << std::endl;
    }
}
