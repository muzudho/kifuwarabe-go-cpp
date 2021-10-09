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

void Position::PrintSgf()
{
    int i;

    // ヘッダー出力
    Prt("(;GM[1]SZ[%d]KM[%.1f]PB[]PW[]\n", kBoardSize, komi);

    // 指し手出力
    for (i = 0; i < moves; i++)
    {
        // 座標
        int z = record[i];

        // 段
        int y = z / kWidth;

        // 筋
        int x = z - y * kWidth;

        // 色
        const char* sStone[2] = { "B", "W" };
        Prt(";%s", sStone[i & 1]);

        // パス
        if (z == 0)
        {
            Prt("[]");
        }
        else
        {
            Prt("[%c%c]", x + 'a' - 1, y + 'a' - 1);
        }

        // 改行
        if (((i + 1) % 10) == 0)
            Prt("\n");
    }

    // 終端
    Prt(")\n");
}

double Position::CountTotalTime()
{
    int i;
    double total_time[2];

    total_time[0] = 0; // black time
    total_time[1] = 0; // white

    for (i = 0; i < moves; i++)
    {
        total_time[i & 1] += record_time[i];
    }

    return total_time[moves & 1];
}

int Position::CountScore(int turn_color)
{
    // 筋
    int x;

    // 段
    int y;

    // ループ カウンター
    int i;

    // 黒のスコア
    int score = 0;

    // 手番の勝ちなら1、負けなら0
    int win;

    // 黒石の数、白石の数
    int black_area = 0, white_area = 0;

    // 石の数＋地の数
    int black_sum, white_sum;

    // 4方向にある石について、[ゴミ値, 盤上の黒石の数, 盤上の白石の数]
    int mk[4];

    // [空点の数, 黒石の数, 白石の数]
    int kind[3];

    kind[0] = kind[1] = kind[2] = 0;
    for (y = 0; y < kBoardSize; y++)
        for (x = 0; x < kBoardSize; x++)
        {
            int z = GetZ(x + 1, y + 1);
            int c = Board[z];
            kind[c]++;

            // 石が置いてある座標なら以降は無視
            if (c != 0)
            {
                if (c == 1)
                    board_area_sum[z]++;
                if (c == 2)
                    board_area_sum[z]--;
                continue;
            }

            // 大雑把に さくっと地計算。
            // 4方向にある黒石、白石の数を数えます
            mk[1] = mk[2] = 0;
            for (i = 0; i < 4; i++)
                mk[Board[z + kDir4[i]]]++;

            // 黒石だけがあるなら黒の地
            if (mk[1] && mk[2] == 0)
            {
                black_area++;
                board_area_sum[z]++;
            }

            // 白石だけがあるなら白の地
            if (mk[2] && mk[1] == 0)
            {
                white_area++;
                board_area_sum[z]--;
            }
        }

    // スコア計算（コミ含まず）
    black_sum = kind[1] + black_area;
    white_sum = kind[2] + white_area;
    score = black_sum - white_sum;

    // 黒の勝敗判定
    win = 0;
    if (score - komi > 0)
        win = 1;

    if (win == 1)
    {
        // 黒番の勝ちなら
        // black win
        for (y = 0; y < kBoardSize; y++)
            for (x = 0; x < kBoardSize; x++)
            {
                // 着手点
                int z = GetZ(x + 1, y + 1);

                // 同色
                if (Board[z] == 1)
                    // 集計
                    board_winner[0][z]++;
            }

        // 黒勝ちのカウント
        winner_count[0]++;
    }
    else
    {
        // 白番の勝ちなら
        for (y = 0; y < kBoardSize; y++)
            for (x = 0; x < kBoardSize; x++)
            {
                // 着手点
                int z = GetZ(x + 1, y + 1);

                // 同色
                if (Board[z] == 2)
                    // 集計
                    board_winner[1][z]++;
            }

        // 白勝ちのカウント
        winner_count[1]++;
    }

    // 白番なら勝敗を反転
    if (turn_color == 2)
        win = -win;

    //Prt("black_sum=%2d, (stones=%2d, area=%2d)\n",black_sum, kind[1], black_area);
    //Prt("white_sum=%2d, (stones=%2d, area=%2d)\n",white_sum, kind[2], white_area);
    //Prt("score=%d, win=%d\n",score, win);
    return win;
}

void Position::TakeStone(int tz, int color)
{
    int z, i;

    hashCode.HashXor(tz, color);
    Board[tz] = 0;
    for (i = 0; i < 4; i++)
    {
        z = tz + kDir4[i];
        if (Board[z] == color)
            TakeStone(z, color);
    }
}
