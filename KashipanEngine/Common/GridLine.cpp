#include "3d/PrimitiveDrawer.h"
#include "Base/Renderer.h"
#include "GridLine.h"

namespace KashipanEngine {

GridLine::GridLine(GridLineType type, float gridSize, UINT axisLineSideCount) {
    type_ = type;
    gridSize_ = gridSize;
    axisLineSideCount_ = axisLineSideCount;
    axisLineCount_ = axisLineSideCount * 2 + 1;
    gridLineCount_ = axisLineCount_ * 2;
    vertexCount_ = gridLineCount_ * 2;
    indexCount_ = gridLineCount_ * 2;

    mesh_ = PrimitiveDrawer::CreateMesh<VertexDataLine>(vertexCount_, indexCount_, sizeof(VertexDataLine));

    switch (type_) {
        case KashipanEngine::GridLineType::XZ:
            GenerateGridXZ();
            break;
        case KashipanEngine::GridLineType::XY:
            GenerateGridXY();
            break;
        case KashipanEngine::GridLineType::YZ:
            GenerateGridYZ();
            break;
    }

    transformationMatrixResource_ = PrimitiveDrawer::CreateBufferResources(sizeof(TransformationMatrix));
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixMap_));
    lineOptionResource_ = PrimitiveDrawer::CreateBufferResources(sizeof(LineOption));
    lineOptionResource_->Map(0, nullptr, reinterpret_cast<void **>(&lineOptionMap_));
    lineOptionMap_->type = kLineThickness;
}

void GridLine::Draw() const {
    Renderer::LineState lineState;
    lineState.mesh = mesh_.get();
    lineState.transformationMatrixResource = transformationMatrixResource_.Get();
    lineState.transformationMatrixMap = transformationMatrixMap_;
    lineState.lineOptionResource = lineOptionResource_.Get();
    lineState.lineOptionMap = lineOptionMap_;
    lineState.indexCount = indexCount_;
    lineState.vertexCount = vertexCount_;
    lineState.lineType = kLineThickness;
    lineState.isUseCamera = true;
    renderer_->DrawSetLine(lineState);
}

void GridLine::GenerateGridXZ() {
    Vector3 offset = {
        -gridSize_ * static_cast<float>(axisLineSideCount_),
        0.0f,
        -gridSize_ * static_cast<float>(axisLineSideCount_)
    };
    float lineLengthHalf = gridSize_ * static_cast<float>(axisLineSideCount_);
    int centerLine = static_cast<int>(axisLineSideCount_);
    
    for (int i = 0; i < static_cast<int>(axisLineCount_); i++) {
        mesh_->indexBufferMap[i * 2 + 0] = i * 2 + 0;
        mesh_->indexBufferMap[i * 2 + 1] = i * 2 + 1;

        float xOffset = offset.x + gridSize_ * static_cast<float>(i);
        mesh_->vertexBufferMap[i * 2 + 0] = {
            { xOffset, offset.y, -lineLengthHalf, 1.0f },
            kNormalLineVertexData.color,
            kNormalLineVertexData.width, 0.0f, 0.0f
        };
        mesh_->vertexBufferMap[i * 2 + 1] = {
            { xOffset, offset.y, lineLengthHalf, 1.0f },
            kNormalLineVertexData.color,
            kNormalLineVertexData.width, 0.0f, 0.0f
        };

        // 特定の間隔で線の色を変える
        if ((i - centerLine) % static_cast<int>(kGridLineColorChangeInterval) == 0) {
            // 緑色に設定
            mesh_->vertexBufferMap[i * 2 + 0].color = {
                0.0f,
                kIntervalGridLineVertexData.color.y,
                0.0f,
                kIntervalGridLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[i * 2 + 1].color = {
                0.0f,
                kIntervalGridLineVertexData.color.y,
                0.0f,
                kIntervalGridLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[i * 2 + 0].width = kIntervalGridLineVertexData.width;
            mesh_->vertexBufferMap[i * 2 + 1].width = kIntervalGridLineVertexData.width;
        }
        // 中心線の場合ははっきりとした色にする
        if (i == centerLine) {
            // 緑色に設定
            mesh_->vertexBufferMap[i * 2 + 0].color = {
                0.0f,
                kCenterLineVertexData.color.y,
                0.0f,
                kCenterLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[i * 2 + 1].color = {
                0.0f,
                kCenterLineVertexData.color.y,
                0.0f,
                kCenterLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[i * 2 + 0].width = kCenterLineVertexData.width;
            mesh_->vertexBufferMap[i * 2 + 1].width = kCenterLineVertexData.width;
        }
    }

    UINT indexOffset = axisLineCount_ * 2;
    for (int i = 0; i < static_cast<int>(axisLineCount_); i++) {
        mesh_->indexBufferMap[indexOffset + i * 2 + 0] = indexOffset + i * 2 + 0;
        mesh_->indexBufferMap[indexOffset + i * 2 + 1] = indexOffset + i * 2 + 1;

        float zOffset = offset.z + gridSize_ * static_cast<float>(i);
        mesh_->vertexBufferMap[indexOffset + i * 2 + 0] = {
            { -lineLengthHalf, offset.y, zOffset, 1.0f },
            kNormalLineVertexData.color,
            0.0f, 0.0f, kNormalLineVertexData.depth
        };
        mesh_->vertexBufferMap[indexOffset + i * 2 + 1] = {
            { lineLengthHalf, offset.y, zOffset, 1.0f },
            kNormalLineVertexData.color,
            0.0f, 0.0f, kNormalLineVertexData.depth
        };

        // 特定の間隔で線の色を変える
        if ((i - centerLine) % static_cast<int>(kGridLineColorChangeInterval) == 0) {
            // 赤色に設定
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].color = {
                kIntervalGridLineVertexData.color.x,
                0.0f,
                0.0f,
                kIntervalGridLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].color = {
                kIntervalGridLineVertexData.color.x,
                0.0f,
                0.0f,
                kIntervalGridLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].depth = kIntervalGridLineVertexData.depth;
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].depth = kIntervalGridLineVertexData.depth;
        }
        // 中心線の場合ははっきりとした色にする
        if (i == centerLine) {
            // 赤色に設定
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].color = {
                kCenterLineVertexData.color.x,
                0.0f,
                0.0f,
                kCenterLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].color = {
                kCenterLineVertexData.color.x,
                0.0f,
                0.0f,
                kCenterLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].depth = kCenterLineVertexData.depth;
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].depth = kCenterLineVertexData.depth;
        }
    }
}

void GridLine::GenerateGridXY() {
    Vector3 offset = {
        -gridSize_ * static_cast<float>(axisLineSideCount_),
        -gridSize_ * static_cast<float>(axisLineSideCount_),
        0.0f
    };
    float lineLengthHalf = gridSize_ * static_cast<float>(axisLineSideCount_);
    int centerLine = static_cast<int>(axisLineSideCount_);

    for (int i = 0; i < static_cast<int>(axisLineCount_); i++) {
        mesh_->indexBufferMap[i * 2 + 0] = i * 2 + 0;
        mesh_->indexBufferMap[i * 2 + 1] = i * 2 + 1;

        float xOffset = offset.x + gridSize_ * static_cast<float>(i);
        mesh_->vertexBufferMap[i * 2 + 0] = {
            { xOffset, -lineLengthHalf, offset.z, 1.0f },
            kNormalLineVertexData.color,
            kNormalLineVertexData.width, 0.0f, 0.0f
        };
        mesh_->vertexBufferMap[i * 2 + 1] = {
            { xOffset, lineLengthHalf, offset.z, 1.0f },
            kNormalLineVertexData.color,
            kNormalLineVertexData.width, 0.0f, 0.0f
        };
        
        // 特定の間隔で線の色を変える
        if ((i - centerLine) % static_cast<int>(kGridLineColorChangeInterval) == 0) {
            // 青色に設定
            mesh_->vertexBufferMap[i * 2 + 0].color = {
                0.0f,
                0.0f,
                kIntervalGridLineVertexData.color.z,
                kIntervalGridLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[i * 2 + 1].color = {
                0.0f,
                0.0f,
                kIntervalGridLineVertexData.color.z,
                kIntervalGridLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[i * 2 + 0].width = kIntervalGridLineVertexData.width;
            mesh_->vertexBufferMap[i * 2 + 1].width = kIntervalGridLineVertexData.width;
        }
        // 中心線の場合ははっきりとした色にする
        if (i == centerLine) {
            // 青色に設定
            mesh_->vertexBufferMap[i * 2 + 0].color = {
                0.0f,
                0.0f,
                kCenterLineVertexData.color.z,
                kCenterLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[i * 2 + 1].color = {
                0.0f,
                0.0f,
                kCenterLineVertexData.color.z,
                kCenterLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[i * 2 + 0].width = kCenterLineVertexData.width;
            mesh_->vertexBufferMap[i * 2 + 1].width = kCenterLineVertexData.width;
        }
    }

    UINT indexOffset = axisLineCount_ * 2;
    for (int i = 0; i < static_cast<int>(axisLineCount_); i++) {
        mesh_->indexBufferMap[indexOffset + i * 2 + 0] = indexOffset + i * 2 + 0;
        mesh_->indexBufferMap[indexOffset + i * 2 + 1] = indexOffset + i * 2 + 1;

        float yOffset = offset.y + gridSize_ * static_cast<float>(i);
        mesh_->vertexBufferMap[indexOffset + i * 2 + 0] = {
            { -lineLengthHalf, yOffset, offset.z, 1.0f },
            kNormalLineVertexData.color,
            0.0f, kNormalLineVertexData.height, 0.0f
        };
        mesh_->vertexBufferMap[indexOffset + i * 2 + 1] = {
            { lineLengthHalf, yOffset, offset.z, 1.0f },
            kNormalLineVertexData.color,
            0.0f, kNormalLineVertexData.height, 0.0f
        };
        
        // 特定の間隔で線の色を変える
        if ((i - centerLine) % static_cast<int>(kGridLineColorChangeInterval) == 0) {
            // 赤色に設定
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].color = {
                kIntervalGridLineVertexData.color.x,
                0.0f,
                0.0f,
                kIntervalGridLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].color = {
                kIntervalGridLineVertexData.color.x,
                0.0f,
                0.0f,
                kIntervalGridLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].height = kIntervalGridLineVertexData.height;
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].height = kIntervalGridLineVertexData.height;
        }
        // 中心線の場合ははっきりとした色にする
        if (i == centerLine) {
            // 赤色に設定
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].color = {
                kCenterLineVertexData.color.x,
                0.0f,
                0.0f,
                kCenterLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].color = {
                kCenterLineVertexData.color.x,
                0.0f,
                0.0f,
                kCenterLineVertexData.color.w,
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].height = kCenterLineVertexData.height;
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].height = kCenterLineVertexData.height;
        }
    }
}

void GridLine::GenerateGridYZ() {
    Vector3 offset = {
        0.0f,
        -gridSize_ * static_cast<float>(axisLineSideCount_),
        -gridSize_ * static_cast<float>(axisLineSideCount_)
    };
    float lineLengthHalf = gridSize_ * static_cast<float>(axisLineSideCount_);
    int centerLine = static_cast<int>(axisLineSideCount_);

    for (int i = 0; i < static_cast<int>(axisLineCount_); i++) {
        mesh_->indexBufferMap[i * 2 + 0] = i * 2 + 0;
        mesh_->indexBufferMap[i * 2 + 1] = i * 2 + 1;
        
        float yOffset = offset.y + gridSize_ * static_cast<float>(i);
        mesh_->vertexBufferMap[i * 2 + 0] = {
            { offset.x, yOffset, -lineLengthHalf, 1.0f },
            kNormalLineVertexData.color,
            0.0f, kNormalLineVertexData.height, 0.0f
        };
        mesh_->vertexBufferMap[i * 2 + 1] = {
            { offset.x, yOffset, lineLengthHalf, 1.0f },
            kNormalLineVertexData.color,
            0.0f, kNormalLineVertexData.height, 0.0f
        };
        
        // 特定の間隔で線の色を変える
        if ((i - centerLine) % static_cast<int>(kGridLineColorChangeInterval) == 0) {
            // 緑色に設定
            mesh_->vertexBufferMap[i * 2 + 0].color = {
                0.0f,
                kIntervalGridLineVertexData.color.y,
                0.0f,
                kIntervalGridLineVertexData.color.w
            };
            mesh_->vertexBufferMap[i * 2 + 1].color = {
                0.0f,
                kIntervalGridLineVertexData.color.y,
                0.0f,
                kIntervalGridLineVertexData.color.w
            };
            mesh_->vertexBufferMap[i * 2 + 0].height = kIntervalGridLineVertexData.height;
            mesh_->vertexBufferMap[i * 2 + 1].height = kIntervalGridLineVertexData.height;
        }
        // 中心線の場合ははっきりとした色にする
        if (i == centerLine) {
            // 緑色に設定
            mesh_->vertexBufferMap[i * 2 + 0].color = {
                0.0f,
                kCenterLineVertexData.color.y,
                0.0f,
                kCenterLineVertexData.color.w
            };
            mesh_->vertexBufferMap[i * 2 + 1].color = {
                0.0f,
                kCenterLineVertexData.color.y,
                0.0f,
                kCenterLineVertexData.color.w
            };
            mesh_->vertexBufferMap[i * 2 + 0].height = kCenterLineVertexData.height;
            mesh_->vertexBufferMap[i * 2 + 1].height = kCenterLineVertexData.height;
        }
    }

    UINT indexOffset = axisLineCount_ * 2;
    for (int i = 0; i < static_cast<int>(axisLineCount_); i++) {
        mesh_->indexBufferMap[indexOffset + i * 2 + 0] = indexOffset + i * 2 + 0;
        mesh_->indexBufferMap[indexOffset + i * 2 + 1] = indexOffset + i * 2 + 1;
        
        float zOffset = offset.z + gridSize_ * static_cast<float>(i);
        mesh_->vertexBufferMap[indexOffset + i * 2 + 0] = {
            { offset.x, -lineLengthHalf, zOffset, 1.0f },
            kNormalLineVertexData.color,
            0.0f, 0.0f, kNormalLineVertexData.depth
        };
        mesh_->vertexBufferMap[indexOffset + i * 2 + 1] = {
            { offset.x, lineLengthHalf, zOffset, 1.0f },
            kNormalLineVertexData.color,
            0.0f, 0.0f, kNormalLineVertexData.depth
        };

        // 特定の間隔で線の色を変える
        if ((i - centerLine) % static_cast<int>(kGridLineColorChangeInterval) == 0) {
            // 青色に設定
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].color = {
                0.0f,
                0.0f,
                kIntervalGridLineVertexData.color.z,
                kIntervalGridLineVertexData.color.w
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].color = {
                0.0f,
                0.0f,
                kIntervalGridLineVertexData.color.z,
                kIntervalGridLineVertexData.color.w
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].depth = kIntervalGridLineVertexData.depth;
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].depth = kIntervalGridLineVertexData.depth;
        }
        // 中心線の場合ははっきりとした色にする
        if (i == centerLine) {
            // 青色に設定
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].color = {
                0.0f,
                0.0f,
                kCenterLineVertexData.color.z,
                kCenterLineVertexData.color.w
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].color = {
                0.0f,
                0.0f,
                kCenterLineVertexData.color.z,
                kCenterLineVertexData.color.w
            };
            mesh_->vertexBufferMap[indexOffset + i * 2 + 0].depth = kCenterLineVertexData.depth;
            mesh_->vertexBufferMap[indexOffset + i * 2 + 1].depth = kCenterLineVertexData.depth;
        }
    }
}

} // namespace KashipanEngine