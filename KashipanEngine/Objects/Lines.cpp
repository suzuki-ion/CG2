#include "3d/PrimitiveDrawer.h"
#include "Base/Renderer.h"
#include "Lines.h"

namespace KashipanEngine {

Lines::Lines(const int lineCount) {
    vertexCount_ = lineCount + 1;
    indexCount_ = lineCount * 2;

    pipeLineSet_ = PrimitiveDrawer::CreateLinePipeline();
    mesh_ = PrimitiveDrawer::CreateMesh<VertexDataLine>(vertexCount_, indexCount_, sizeof(VertexDataLine));

    transformationMatrixResource_ = PrimitiveDrawer::CreateBufferResources(sizeof(TransformationMatrix));
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixMap_));

    statePtr_.vertexData = mesh_->vertexBufferMap;

    for (int i = 0; i < vertexCount_ - 1; i++) {
        mesh_->indexBufferMap[(i * 2) + 0] = i + 0;
        mesh_->indexBufferMap[(i * 2) + 1] = i + 1;
    }

    for (int i = 0; i < vertexCount_; i++) {
        mesh_->vertexBufferMap[i].pos = { 0.0f, 0.0f, 0.0f, 1.0f };
        mesh_->vertexBufferMap[i].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        mesh_->vertexBufferMap[i].width = 0.1f;
        mesh_->vertexBufferMap[i].height = 0.0f;
        mesh_->vertexBufferMap[i].depth = 0.1f;
    }
}

void Lines::SetWVPMatrix(const Matrix4x4 &wvpMatrix) {
    transformationMatrixMap_->wvp = wvpMatrix;
}

void Lines::Draw() const {
    Renderer::LineState lineState;
    lineState.mesh = mesh_.get();
    lineState.transformationMatrixResource = transformationMatrixResource_.Get();
    lineState.transformationMatrixMap = transformationMatrixMap_;
    lineState.indexCount = indexCount_;
    lineState.vertexCount = vertexCount_;
    lineState.isUseCamera = true;
    renderer_->DrawSetLine(lineState);
}

} // namespace KashipanEngine