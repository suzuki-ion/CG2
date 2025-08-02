#pragma once
#include <memory>
#include "3d/PrimitiveDrawer.h"
#include "Common/PipeLineSet.h"
#include "Common/VertexDataLine.h"
#include "Common/LineOption.h"
#include "Common/Mesh.h"
#include "Common/TransformationMatrix.h"

namespace KashipanEngine {

class Renderer;

struct Lines {
public:
    struct StatePtr {
        VertexDataLine *vertexData;
    };

    Lines() = delete;
    Lines(const int lineCount, LineType lineType = kLineNormal);

    void SetRenderer(Renderer *renderer) {
        renderer_ = renderer;
    }
    void SetLineType(LineType lineType) {
        lineType_ = lineType;
        lineOptionMap_->type = lineType_;
    }
    void SetWVPMatrix(const Matrix4x4 &wvpMatrix);

    StatePtr GetStatePtr() {
        return statePtr_;
    }

    /// @brief 描画処理
    void Draw() const;

private:
    StatePtr statePtr_;

    UINT vertexCount_ = 0;
    UINT indexCount_ = 0;

    Renderer *renderer_ = nullptr;
    LineType lineType_ = kLineNormal;

    std::unique_ptr<Mesh<VertexDataLine>> mesh_;
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> lineOptionResource_;
    TransformationMatrix *transformationMatrixMap_ = nullptr;
    LineOption *lineOptionMap_ = nullptr;
};

} // namespace KashipanEngine