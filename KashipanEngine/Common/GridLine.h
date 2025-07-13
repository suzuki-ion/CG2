#pragma once
#include <d3d12.h>
#include <memory>
#include "PipeLineSet.h"
#include "VertexDataLine.h"
#include "Mesh.h"
#include "TransformationMatrix.h"

namespace KashipanEngine {

// 前方宣言
class Renderer;

enum class GridLineType {
    XZ, // XZ平面
    XY, // XY平面
    YZ, // YZ平面
};

class GridLine {
public:
    /// @brief グリッド線の色が変化する間隔
    static const UINT kGridLineColorChangeInterval = 10;

    GridLine() = delete;
    /// @brief コンストラクタ
    /// @param type グリッドの種類
    /// @param gridHalfSize 
    /// @param gridLineSideHalfCount 1軸上における線の片側の数
    GridLine(GridLineType type, float gridSize, UINT axisLineSideCount);

    void SetRenderer(Renderer *renderer) {
        renderer_ = renderer;
    }

    void Draw() const;

private:
    void GenerateGridXZ();
    void GenerateGridXY();
    void GenerateGridYZ();

    Renderer *renderer_ = nullptr;
    PipeLineSet pipeLineSet_;

    GridLineType type_ = GridLineType::XZ;
    float gridSize_ = 0.0f;
    
    /// @brief 1軸上における線の片側の数
    UINT axisLineSideCount_ = 0;
    /// @brief 1軸上における線の本数
    UINT axisLineCount_ = 0;
    /// @brief 線の総数
    UINT gridLineCount_ = 0;

    UINT vertexCount_ = 0;
    UINT indexCount_ = 0;
    
    std::unique_ptr<Mesh<VertexDataLine>> mesh_;
    Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
    TransformationMatrix *transformationMatrixMap_ = nullptr;
};

} // namespace KashipanEngine