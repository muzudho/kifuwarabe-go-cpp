#include "common.h"
#include "position.h"
#include <stdlib.h>

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

    Prt("   ");
    //for (x=0;x<kBoardSize;x++) Prt("%d",x+1);
    for (x = 0; x < kBoardSize; x++)
        Prt("%c", 'A' + x + (x > 7));
    Prt("\n");
    for (y = 0; y < kBoardSize; y++)
    {
        //  Prt("%2d ",y+1);
        Prt("%2d ", kBoardSize - y);
        for (x = 0; x < kBoardSize; x++)
        {
            Prt("%s", str[Board[GetZ(x + 1, y + 1)]]);
        }
        if (y == 4)
            Prt("  ko_z=%s,moves=%d", GetCharZ(ko_z), moves);
        if (y == 7)
            Prt("  play_z=%s, color=%d", GetCharZ(played_z), color);

        Prt("\n");
    }
}

void Position::AddMoves(int z, int color, double sec)
{
    // 石を置きます
    int err = PutStone(z, color, kFillEyeOk);

    // 非合法手なら強制終了
    if (err != 0)
    {
        Prt("PutStone err=%d\n", err);
        exit(0);
    }

    // 棋譜の末尾に記入
    record[moves] = z;
    record_time[moves] = sec;

    // 棋譜の書くところを１つ進めます
    moves++;

    // 盤表示
    PrintBoard();

    // ハッシュコード表示
    Prt("hashcode=");
    PrtCode64(hashCode.hashcode);
    Prt("\n");
}
