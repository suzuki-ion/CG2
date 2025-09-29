#pragma once

// ディスクリプタヒープクラスの便利なインクルードヘッダー
#include "RTVHeap.h"
#include "DSVHeap.h"
#include "SRVHeap.h"

namespace KashipanEngine {
    // 全てのディスクリプタヒープクラスがここに含まれます
    // - RTVHeap: レンダーターゲットビュー用
    // - DSVHeap: デプスステンシルビュー用  
    // - SRVHeap: シェーダーリソースビュー、コンスタントバッファビュー、アンオーダードアクセスビュー用
}