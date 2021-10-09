#pragma once

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
    int SelectBestUcb(int node_n, int color);

    /// <summary>
    /// レーブの更新？
    /// </summary>
    /// <param name="pN">ノード？</param>
    /// <param name="color">手番の色</param>
    /// <param name="current_depth">経路の深さ</param>
    /// <param name="win">勝率</param>
    void UpdateRave(Node* pN, int color, int current_depth, double win);

    int SearchUct(int color, int node_n);
    int GetBestUct(int color);
};
