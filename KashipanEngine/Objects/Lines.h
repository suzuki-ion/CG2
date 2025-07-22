#pragma once
#include <memory>
#include "Common/PipeLineSet.h"
#include "Common/VertexDataLine.h"
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
    Lines(const int lineCount);

    void SetRenderer(Renderer *renderer) {
        renderer_ = renderer;
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
    PipeLineSet pipeLineSet_;

    std::unique_ptr<Mesh<VertexDataLine>> mesh_;
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
    TransformationMatrix *transformationMatrixMap_ = nullptr;
};

} // namespace KashipanEngine