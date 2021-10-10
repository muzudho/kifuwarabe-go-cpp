#pragma once

#include <stdlib.h>
#include "common.h"
#include "hashCode.h"
#include "go15.h"
#include "liberty.h"

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

extern TimeMan timeMan;
extern int depth;

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
    double komi = 6.5;

    /// <summary>
    /// 盤
    /// </summary>
    int Board[kBoardMax] = {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};

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

    /// <summary>
    /// 盤の描画
    /// </summary>
    void PrintBoard();

    /// <summary>
    /// 盤領域を 標準エラー出力へ出力？
    /// </summary>
    void PrintBoardArea();

    /// <summary>
    /// 致命さ？
    /// </summary>
    /// <param name="z">着手点</param>
    /// <returns>致命さを表す数？</returns>
    double GetCriticality(int z);

    /// <summary>
    /// 致命さをエラー出力へ出力？
    /// </summary>
    void PrintCriticality();

    /// <summary>
    /// 石を取り上げます
    /// </summary>
    /// <param name="tz">着手座標</param>
    /// <param name="color">石の色</param>
    void TakeStone(int tz, int color);

    /// <summary>
    /// put stone.
    /// </summary>
    /// <param name="tz">着手座標。0ならパス</param>
    /// <param name="color">石の色</param>
    /// <param name="fill_eye_err">目潰しをエラーとするなら1、そうでないなら0</param>
    /// <returns>エラーコード。success returns 0. in playout, fill_eye_err = 1</returns>
    int PutStone(int tz, int color, int fill_eye_err);

    /// <summary>
    /// 地の簡易計算（これが厳密に計算できるようなら囲碁は完全解明されている）を表示し、勝敗を返します。
    /// スコアにはコミは含みませんが、勝敗にはコミを含んでいます。
    /// </summary>
    /// <param name="turn_color">手番の色</param>
    /// <returns>黒の勝ちなら1、負けなら0</returns>
    int CountScore(int turn_color);

    int Playout(int turn_color);
    int PrimitiveMonteCalro(int color);

    /// <summary>
    /// 消費時間合計
    /// </summary>
    /// <returns></returns>
    double CountTotalTime();

    /// <summary>
    /// 盤の初期化
    /// </summary>
    void InitBoard();

    /// <summary>
    /// 指し手を棋譜に記入
    /// </summary>
    /// <param name="z">座標</param>
    /// <param name="color">手番の色</param>
    void AddMoves(int z, int color, double sec);

    /// <summary>
    /// print SGF game record
    /// </summary>
    void PrintSgf();

    void ClearBeforeComputerMove();

    /// <summary>
    /// 消費時間を棋譜に記録します
    /// </summary>
    /// <param name="color"></param>
    /// <param name="z"></param>
    void SetUpExpendTime(int color, int z);

    /// <summary>
    /// 現図をバックアップ
    /// </summary>
    void BackupCurrent(int* board_copy, int* ko_z_copy);
    /// <summary>
    /// 現図をレストア
    /// </summary>
    void RestoreCurrent(int* board_copy, int* ko_z_copy);
};
