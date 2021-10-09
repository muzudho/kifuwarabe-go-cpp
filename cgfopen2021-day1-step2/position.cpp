#include "common.h"
#include "position.h"
#include <stdlib.h>

/// <summary>
/// 盤の初期化
/// </summary>
void Position::InitBoard()
{
    int i, x, y;

    // 盤の枠を整えます
    for (i = 0; i < kBoardMax; i++)
        Board[i] = 3;

    // 盤上の石をすべて取り除きます
    for (y = 0; y < kBoardSize; y++)
        for (x = 0; x < kBoardSize; x++)
            Board[GetZ(x + 1, y + 1)] = 0;

    // 手数とコウとハッシュコードをクリアーします
    moves = 0;
    ko_z = 0;
    hashCode.hashcode = 0;
}

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

void Position::PrintBoardArea()
{
    int x, y;
    int all = all_playouts;
    if (all == 0)
        all = 1;

    Prt("board_area_sum\n   ");
    for (x = 0; x < kBoardSize; x++)
        Prt("   %c", 'A' + x + (x > 7));
    Prt("\n");
    for (y = 0; y < kBoardSize; y++)
    {
        Prt("%2d ", kBoardSize - y);
        for (x = 0; x < kBoardSize; x++)
        {
            int sum = board_area_sum[GetZ(x + 1, y + 1)];
            double per = 100.0 * sum / all;
            Prt("%4.0f", per);
        }

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

void Position::PrintCriticality()
{
    int x, y;

    Prt("criticality\n  ");
    for (x = 0; x < kBoardSize; x++)
        Prt("    %c", 'A' + x + (x > 7));
    Prt("\n");
    for (y = 0; y < kBoardSize; y++)
    {
        Prt("%2d ", kBoardSize - y);
        for (x = 0; x < kBoardSize; x++)
        {
            double crt = GetCriticality(GetZ(x + 1, y + 1));
            Prt("%5.2f", crt);
        }
        Prt("\n");
    }
}

double Position::GetCriticality(int z)
{
    double all = all_playouts + 1;
    double v = board_winner[0][z] + board_winner[1][z];
    double per = v / all;
    double bp = (double)board_winner[0][z] * winner_count[0] / (all * all);
    double wp = (double)board_winner[1][z] * winner_count[1] / (all * all);
    double criticality = (per - (bp + wp));
    return criticality;
}

int Position::PutStone(int tz, int color, int fill_eye_err)
{
    // 検索情報を覚えておく配列
    int around[4][3];

    // 相手の石の色
    int un_col = FlipColor(color);

    // 空白に石を置いたら1
    int space = 0;

    // 壁に石を置いたら1
    int wall = 0;

    // 自殺手になってしまうとき1
    int mycol_safe = 0;

    // 取り上げた石の数
    int capture_sum = 0;

    // コウかもしれないとき1
    int ko_maybe = 0;

    // 呼吸点の数
    int liberty;

    // 連の石の数
    int stone;

    // ループ・カウンタ
    int i;

    // pass
    if (tz == 0)
    {
        if (ko_z != 0)
            hashCode.HashXor(ko_z, kHashKo);

        ko_z = 0;
        hashCode.HashPass();
        return 0;
    }

    // count 4 neighbor's liberty and stones.
    for (i = 0; i < 4; i++)
    {
        int z, c, liberty, stone;
        around[i][0] = around[i][1] = around[i][2] = 0;

        // 隣の座標
        z = tz + kDir4[i];
        c = Board[z]; // color

        // もし、隣が空点なら
        if (c == 0)
            space++;

        // もし、隣が壁なら
        if (c == 3)
            wall++;

        // もし、隣が空点または壁なら
        if (c == 0 || c == 3)
            continue;

        // 呼吸点の数と、連の石の数を数えます
        CountLiberty(z, &liberty, &stone);

        // 隣の石が相手の色で、呼吸点が1なら、その石を取れます
        around[i][0] = liberty;
        around[i][1] = stone;
        around[i][2] = c;

        // 隣の石が相手の色で、呼吸点が1なら、その石を取れます
        if (c == un_col && liberty == 1)
        {
            capture_sum += stone;
            ko_maybe = z;
        }

        // もし隣に自分の色の石があっても、その石の呼吸点が２以上あればセーフ
        if (c == color && liberty >= 2)
            mycol_safe++;
    }

    // 石を取っておらず、隣に空点がなく、隣に呼吸点が２つ以上空いている自分の石もないなら、自殺手
    if (capture_sum == 0 && space == 0 && mycol_safe == 0)
        return 1; // suicide

    // もし、コウの座標に石を置こうとしたら、コウ
    if (tz == ko_z)
        return 2; // ko

    // もし、目の座標に石を置こうとしたら、目潰し
    if (wall + mycol_safe == 4 && fill_eye_err)
        return 3; // eye

    // もし、石の上に石を置こうとしたら、反則手
    if (Board[tz] != 0)
        return 4;

    // 取れる相手の石を取ります
    for (i = 0; i < 4; i++)
    {
        int lib = around[i][0];
        int c = around[i][2];
        if (c == un_col && lib == 1 && Board[tz + kDir4[i]] != 0)
        {
            TakeStone(tz + kDir4[i], un_col);
        }
    }

    // 石を置きます
    Board[tz] = color;

    // 着手点のビット列を、XOR演算でひっくり返します
    hashCode.HashXor(tz, color);

    // ハッシュコードのビット列の 0,1 をひっくり返している？
    hashCode.HashPass();

    // コウであれば、コウの場所のビット列の 0,1 をひっくり返している？
    if (ko_z != 0)
        hashCode.HashXor(ko_z, kHashKo);

    // 着手点を含む連の呼吸点の数を数えます
    CountLiberty(tz, &liberty, &stone);

    // 石を1個取ったらコウかも知れない
    if (capture_sum == 1 && stone == 1 && liberty == 1)
    {
        ko_z = ko_maybe;
        hashCode.HashXor(ko_z, kHashKo);
    }
    else
    {
        ko_z = 0;
    }
    return 0;
}
