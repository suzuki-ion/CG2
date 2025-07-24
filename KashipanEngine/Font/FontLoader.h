#pragma once
#include "FontStructs.h"

namespace KashipanEngine {

/// @brief フォントデータ(.fnt)の読み込み
/// @param fntFilePath フォントファイル(.fnt)のパス
/// @return 読み込んだフォントデータ
FontData LoadFNT(const char *fntFilePath);

} // namespace KashipanEngine