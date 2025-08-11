#include "Common/JsoncLoader.h"
#include "Common/Logs.h"
#include "PipeLines/EnumMaps.h"
#include "PipeLineManager.h"

namespace KashipanEngine {
using namespace KashipanEngine::PipeLine;

PipeLineManager::PipeLineManager(DirectXCommon *dxCommon, const std::string &pipeLineSettingsPath) {
    Log("PipeLineManager constructor called.");
    if (!dxCommon) {
        LogSimple("DirectXCommon pointer is null.", kLogLevelFlagError);
        return;
    }
    dxCommon_ = dxCommon;

    LogSimple("Loading PipeLine Presets.");
    Json pipeLineSettings = LoadJsonc(pipeLineSettingsPath);
}

void PipeLineManager::LoadPipeLine4Json(const std::string &jsonPath) {
    Json jsonData = LoadJsonc(jsonPath);
}

} // namespace KashipanEngine