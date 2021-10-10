#include "position.h"
#include <iostream>
#include <iomanip>

extern Liberty libertyClass;

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

void Position::PrintBoardArea()
{
    int x, y;
    int all = all_playouts;
    if (all == 0)
        all = 1;

    // 筋符号のリスト
    std::cerr << "board_area_sum" << std::endl << "   ";
    for (x = 0; x < kBoardSize; x++) {
        std::cerr << "   " << (char)('A' + x + (x > 7));
    }
    std::cerr << std::endl;

    // テーブル
    for (y = 0; y < kBoardSize; y++)
    {
        // 行見出し
        std::cerr << std::setw(2) << (kBoardSize - y) << " ";

        for (x = 0; x < kBoardSize; x++)
        {
            int sum = board_area_sum[GetZ(x + 1, y + 1)];
            double per = 100.0 * sum / all;
            std::cerr << std::setw(4) << std::setprecision(0) << per;
        }

        std::cerr << std::endl;
    }
}

void Position::AddMoves(int z, int color, double sec)
{
    // 石を置きます
    int err = PutStone(z, color, kFillEyeOk);

    // 非合法手なら強制終了
    if (err != 0)
    {
        std::cerr << "PutStone err=" << err << std::endl;
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
    std::cerr << "hashcode=";
    PrtCode64(hashCode.hashcode);
    std::cerr << std::endl;
}

void Position::PrintSgf()
{
    int i;

    // ヘッダー出力
    std::cerr << "(;GM[1]SZ[" << kBoardSize << "]KM[" << std::setprecision(1) << komi << "]PB[]PW[]" << std::endl;

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
        std::cerr << ";" << sStone[i & 1];

        // パス
        if (z == 0)
        {
            std::cerr << "[]";
        }
        else
        {
            std::cerr << "[" << (char)(x + 'a' - 1) << (char)(y + 'a' - 1) << "]";
        }

        // 改行
        if (((i + 1) % 10) == 0) {
            std::cerr << std::endl;
        }
    }

    // 終端
    std::cerr << ")" << std::endl;
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

    // std::cerr << "black_sum=" << std::setw( 2 ) << black_sum
    //  << ", (stones=" << std::setw( 2 ) << kind[1]
    //  << ", area=" << std::setw( 2 ) << black_area
    //  << ")" << std::endl;

    // std::cerr << "white_sum=" <<  std::setw( 2 ) <<  white_sum
    //  <<  ", (stones=" <<  std::setw( 2 ) <<   kind[2]
    //  <<  ", area=" <<  std::setw( 2 ) <<  white_area
    //  <<  ")" << std::endl;

    // std::cerr << "score=" << score
    //  <<  ", win=" <<  win << std::endl;
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

    // 筋符号のリスト
    std::cerr << "criticality" << std::endl << "  ";
    for (x = 0; x < kBoardSize; x++) {
        std::cerr << "    " << (char)('A' + x + (x > 7));
    }
    std::cerr << std::endl;

    // テーブル
    for (y = 0; y < kBoardSize; y++)
    {
        // 行見出し
        std::cerr << std::setw(2) << (kBoardSize - y) << " ";

        for (x = 0; x < kBoardSize; x++)
        {
            double crt = GetCriticality(GetZ(x + 1, y + 1));
            std::cerr << std::setw(5) << std::setprecision(2) << crt;
        }

        std::cerr << std::endl;
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
        if (ko_z != 0) {
            hashCode.HashXor(ko_z, kHashKo);
        }

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
        if (c == 0) {
            space++;
        }

        // もし、隣が壁なら
        if (c == 3) {
            wall++;
        }

        // もし、隣が空点または壁なら
        if (c == 0 || c == 3) {
            continue;
        }

        // 呼吸点の数と、連の石の数を数えます
        libertyClass.CountLiberty(z, &liberty, &stone);

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
        if (c == color && liberty >= 2) {
            mycol_safe++;
        }
    }

    // 石を取っておらず、隣に空点がなく、隣に呼吸点が２つ以上空いている自分の石もないなら、自殺手
    if (capture_sum == 0 && space == 0 && mycol_safe == 0) {
        return 1; // suicide
    }

    // もし、コウの座標に石を置こうとしたら、コウ
    if (tz == ko_z) {
        return 2; // ko
    }

    // もし、目の座標に石を置こうとしたら、目潰し
    if (wall + mycol_safe == 4 && fill_eye_err) {
        return 3; // eye
    }

    // もし、石の上に石を置こうとしたら、反則手
    if (Board[tz] != 0) {
        return 4;
    }

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
    if (ko_z != 0) {
        hashCode.HashXor(ko_z, kHashKo);
    }

    // 着手点を含む連の呼吸点の数を数えます
    libertyClass.CountLiberty(tz, &liberty, &stone);

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

void Position::ClearBeforeComputerMove()
{
    // プレイアウト回数
    all_playouts = 0;

    // 盤領域をゼロ クリアー？
    memset(board_area_sum, 0, sizeof(board_area_sum));

    // 盤上の勝ち数をゼロ クリアー？
    memset(board_winner, 0, sizeof(board_winner));

    // 勝ち数をクリアー？
    memset(winner_count, 0, sizeof(winner_count));
}

void Position::SetUpExpendTime(int color, int z) {
    // 秒
    double sec;

    // 消費時間（秒）？
    sec = timeMan.GetSpendTime(timeMan.start_time);

    // 情報表示
    std::cerr << "z=" << GetCharZ(z)
        << ",color=" << color
        << ",moves=" << moves
        << ",playouts=" << all_playouts
        << ", " << std::setprecision(1) << sec
        << " sec(" << std::setprecision(0) << (all_playouts / sec)
        << " po/sec),depth=" << depth
        << std::endl;

    // 指し手を棋譜に記憶します
    AddMoves(z, color, sec);
}

void Position::BackupCurrent(int* board_copy, int* pKo_z_copy) {
    *pKo_z_copy = ko_z;
    memcpy(board_copy, Board, sizeof(Board));
}

void Position::RestoreCurrent(int* board_copy, int* pKo_z_copy) {
    ko_z = *pKo_z_copy;
    memcpy(Board, board_copy, sizeof(Board));
}

/// <summary>
/// 原始モンテカルロ（ネガマックス）
/// </summary>
/// <param name="color">手番の色</param>
/// <returns>最善手の座標</returns>
int Position::PrimitiveMonteCalro(int color)
{
    // Takahashi: 消費時間を短縮したい。でも6秒だとペンキ塗りしてしまう。
    // Takahashi: 8でちょうどいいかと思ったが、石の上に石を置いたかもしれないので、もう少し長めにする。
    // Takahashi: 20だと 持ち時間 30分でちょうど良さそうだが、それでも石の上に石を置いた。
    // number of playout
    int try_num = 15; // 30;

    // 最善手の着手点
    int best_z = 0;

    // 最善の価値
    double best_value;

    // 勝率
    double win_rate;

    // 筋
    int x;

    // 段
    int y;

    // エラーコード
    int err;

    // ループ カウンター
    int i;

    // プレイアウトして勝った回数
    int win_sum;

    // 手番が勝ったら1、負けたら0
    int win;


    // 盤のコピー
    // keep current board
    int board_copy[kBoardMax];
    // コウの座標のコピー
    int ko_z_copy;
    BackupCurrent(board_copy, &ko_z_copy);

    // 根ノードでは最低-100点から
    best_value = -100;

    // try all empty point
    for (y = 0; y < kBoardSize; y++) {
        for (x = 0; x < kBoardSize; x++)
        {
            // 石を置く座標
            int z = GetZ(x + 1, y + 1);

            // 空点でなければ無視
            if (Board[z] != 0) {
                continue;
            }

            // 目潰ししないように石を置く
            err = PutStone(z, color, kFillEyeErr);
            if (err != 0) {
                continue;
            }

            // 勝った回数
            win_sum = 0;
            for (i = 0; i < try_num; i++)
            {
                // 現局面を退避
                int board_copy2[kBoardMax];
                int ko_z_copy2;
                BackupCurrent(board_copy2, &ko_z_copy2);

                // プレイアウト
                win = -Playout(FlipColor(color));
                win_sum += win;

                // 現局面に復元
                RestoreCurrent(board_copy2, &ko_z_copy2);
            }

            // 勝率
            win_rate = (double)win_sum / try_num;
            //  PrintBoard();
            // std::cerr << "z=" << Get81(z) 
            // << ",win=" <<  std::setw( 5 ) << std::setprecision( 3 ) << win_rate << std::endl;

            // 最善手の更新
            if (win_rate > best_value)
            {
                best_value = win_rate;
                best_z = z;
                // std::cerr << "best_z=" << Get81(best_z)
                //  << ",color=" << color
                //  << ",v=" << std::setw( 5 ) << std::setprecision( 3 ) << best_value
                //  << ",try_num=" try_num << std::endl;
            }
        }
    }

    // Takahashi: for文のコードブロックの外に移動
    RestoreCurrent(board_copy, &ko_z_copy);

    return best_z;
}
