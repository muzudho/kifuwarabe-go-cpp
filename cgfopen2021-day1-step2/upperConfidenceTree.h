#pragma once

#include "common.h"
#include "node.h"

/// <summary>
/// 最大10000局面まで
/// </summary>
const int kNodeMax = 10000;

/// <summary>
/// UCTで使われる経路（パス）
/// プレイアウトでも使う
/// </summary>
extern int path[kDMax];

/// <summary>
/// UCTで使われる経路（パス）の先頭がらn番目(0開始)
/// プレイアウトでも使う
/// </summary>
extern int depth;

/// <summary>
/// `UCT` - 探索と知識利用のバランスを取る手法
/// </summary>
class UpperConfidenceTree
{
public:

    /// <summary>
    /// number of uct loop
    /// </summary>
    int loop = 1000000;

    /// <summary>
    /// ノードのリスト
    /// </summary>
    Node nodeList[kNodeMax];

    /// <summary>
    /// ノードのリストのサイズ。登録局面数
    /// </summary>
    int node_num = 0;

    int CreateNode(int prev_z);

    /// <summary>
    /// レーブの更新？
    /// </summary>
    /// <param name="pN">ノード？</param>
    /// <param name="color">手番の色</param>
    /// <param name="current_depth">経路の深さ</param>
    /// <param name="win">勝率</param>
    void UpdateRave(Node* pN, int color, int current_depth, double win);

    /// <summary>
    /// ゲームをプレイします（再帰関数）
    /// UCTという探索の手法で行います
    /// 指定の UCTのループ回数だけ呼び出されます（定数倍）
    /// </summary>
    /// <param name="color">手番の色。最初は考えているプレイヤーの色</param>
    /// <param name="node_n">ノードのリストのインデックス。最初は0</param>
    /// <returns>手番の勝率</returns>
    int SearchUct(Position position, int color, int node_n);

    /// <summary>
    /// UCBが最大の手を返します。
    /// 一度も試していない手は優先的に選びます。
    /// 定数 Ｃ は実験で決めてください。
    /// PASS があるので、すべての手がエラーはありえません。
    /// （UCTループ１回分の中で、定数倍ループ。m×n）
    /// </summary>
    /// <param name="node_n">ノードのリストのインデックス</param>
    /// <param name="color">手番の色</param>
    /// <returns>ノードのリストのインデックス。選択した子ノードを指します</returns>
    int SelectBestUcb(int node_n, int color);
};
