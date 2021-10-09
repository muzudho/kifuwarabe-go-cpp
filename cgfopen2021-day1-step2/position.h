#pragma once

#include "common.h"
#include "hashCode.h"
#include "go15.h"

/// <summary>
/// 右、下、左、上
/// </summary>
const int kDir4[4] = { +1, +kWidth, -1, -kWidth };

/// <summary>
/// 右、下、左、上、右下、左下、左上、右上
/// </summary>
const int kDir8[8] = { +1, +kWidth, -1, -kWidth, +1 + kWidth, +kWidth - 1, -1 - kWidth, -kWidth + 1 };

/// <summary>
/// 指し手の要素数
/// </summary>
const int kMaxMoves = 1000;

/// <summary>
/// 局面
/// </summary>
class Position
{
public:
    /// <summary>
    /// コミ。
    /// CGFオープン2021では、19路では 6.5、 9路では 7
    /// </summary>
    double komi = 7;

    /// <summary>
    /// 盤
    /// </summary>
    int Board[kBoardMax] = {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};

    /// <summary>
    /// コウの座標
    /// </summary>
    int ko_z = 0;

    /// <summary>
    /// 棋譜
    /// </summary>
    int record[kMaxMoves];

    /// <summary>
    /// 指し手の消費時間
    /// </summary>
    double record_time[kMaxMoves];

    /// <summary>
    /// n手目。1手目が0
    /// </summary>
    int moves = 0;

    /// <summary>
    /// プレイアウト回数
    /// </summary>
    int all_playouts = 0;

    /// <summary>
    /// テストでプレイアウトするなら1
    /// </summary>
    int flag_test_playout = 0;

    /// <summary>
    /// 盤エリアの何らかの累計？
    /// </summary>
    int board_area_sum[kBoardMax];

    /// <summary>
    /// 盤の各交点の勝者？
    /// </summary>
    int board_winner[2][kBoardMax];

    /// <summary>
    /// 勝者のカウント？
    /// </summary>
    int winner_count[2];

    /// <summary>
    /// 盤の各交点の致命さ？
    /// </summary>
    int criticality[kBoardMax];

    /// <summary>
    /// 呼吸点を探索するアルゴリズムで使用
    /// </summary>
    int check_board[kBoardMax];

    class HashCode hashCode;

    Position() {
        hashCode = HashCode();
    }

    void PrintBoard();
    void PrintBoardArea();
    double GetCriticality(int z);
    void PrintCriticality();
    void CountLibertySub(int tz, int color, int *p_liberty, int *p_stone);
    void CountLiberty(int tz, int *p_liberty, int *p_stone);
    void TakeStone(int tz, int color);
    int PutStone(int tz, int color, int fill_eye_err);
    int CountScore(int turn_color);
    int Playout(int turn_color);
    int PrimitiveMonteCalro(int color);
    double CountTotalTime();
    int GetBestUct(int color);
    void InitBoard();

    /// <summary>
    /// 指し手を棋譜に記入
    /// </summary>
    /// <param name="z">座標</param>
    /// <param name="color">手番の色</param>
    void AddMoves(int z, int color, double sec);

    int PlayComputerMove(int color, int search);

    /// <summary>
    /// print SGF game record
    /// </summary>
    void PrintSgf();
};
