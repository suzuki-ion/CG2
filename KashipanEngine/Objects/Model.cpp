#include <fstream>
#include <sstream>
#include <cassert>

#include "Model.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Common/Logs.h"
#include "Base/Texture.h"

namespace KashipanEngine {

namespace {

/// @brief 指定のマテリアル情報を取得
/// @param directoryPath ディレクトリのパス
/// @param fileName マテリアルのファイル名
/// @param usemtl 取得したいマテリアル名
/// @return マテリアル情報
MaterialData LoadMaterialFile(const std::string &directoryPath, const std::string &fileName, const std::string &usemtl) {
    MaterialData materialData;                                  // 構築するMaterialData
    std::string line;                                           // ファイルから読み込んだ1行を格納するもの
    std::ifstream materialFile(directoryPath + "/" + fileName); // マテリアルファイル読み込み用
    bool isFound = false;
    
    // ファイルを1行ずつ読み込む
    while (std::getline(materialFile, line)) {
        std::string identifier;
        std::istringstream s(line);
        // 先頭の識別子を読む
        s >> identifier;

        // マテリアル名の情報でなければ除外
        if (identifier != "newmtl") {
            continue;
        }

        // マテリアル名の比較
        std::string mtlName;
        s >> mtlName;
        // 違うなら無視
        if (mtlName != usemtl) {
            continue;
        }
        // あってるならフラグを立ててループを抜ける
        isFound = true;
        break;
    };

    if (!isFound) {
        return materialData;
    }

    // マテリアル情報の取得
    while (std::getline(materialFile, line)) {
        std::string identifier;
        std::istringstream s(line);
        s >> identifier;
        // identifierに応じた処理
        if (identifier == "map_Kd") {
            std::string textureFileName;
            s >> textureFileName;
            // 連結してファイルパスにする
            materialData.textureFilePath = directoryPath + "/" + textureFileName;
            // とりあえずテクスチャ情報だけ欲しいのでここで終了
            return materialData;
        }
    }

    // なにも無かった時用に一応ここでもreturn
    return materialData;
}

} // namespace

ModelData::~ModelData() {
    if (worldTransform_ != nullptr) {
        delete worldTransform_;
        worldTransform_ = nullptr;
    }
}

ModelData::ModelData(ModelData &&other) noexcept {
    if (other.worldTransform_ != nullptr) {
        worldTransform_ = other.worldTransform_;
        other.worldTransform_ = nullptr;
    }

    if (!other.mesh_) {
        return;
    }

    mesh_ = std::move(other.mesh_);
    materialResource_ = other.materialResource_;
    transformationMatrixResource_ = other.transformationMatrixResource_;
    materialMap_ = other.materialMap_;
    transformationMatrixMap_ = other.transformationMatrixMap_;
    vertexCount_ = other.vertexCount_;
    indexCount_ = other.indexCount_;
    useTextureIndex_ = other.useTextureIndex_;
}

void ModelData::CreateData(std::vector<VertexData> &vertexData, std::vector<uint32_t> &indexData, MaterialData &materialData) {
    isUseCamera_ = true;
    // メッシュの生成
    Create(static_cast<UINT>(vertexData.size()), static_cast<UINT>(indexData.size()));
    // メッシュの頂点バッファにデータをコピー
    std::memcpy(mesh_->vertexBufferMap, vertexData.data(), sizeof(VertexData) * vertexData.size());
    // メッシュのインデックスバッファにデータをコピー
    std::memcpy(mesh_->indexBufferMap, indexData.data(), sizeof(uint32_t) * indexData.size());

    // マテリアルの設定
    materialData_ = materialData;
    if (materialData_.textureFilePath.empty()) {
        // テクスチャが指定されていない場合は0を指定
        useTextureIndex_ = 0;
    } else {
        useTextureIndex_ = Texture::Load(materialData_.textureFilePath);
    }

    // ワールド変換データの生成
    worldTransform_ = new WorldTransform();
}

void ModelData::Draw() {
    isUseCamera_ = true;
    DrawCommon(*worldTransform_);
}

void ModelData::Draw(WorldTransform &worldTransform) {
    isUseCamera_ = true;
    DrawCommon(worldTransform);
}

Model::Model(std::string directoryPath, std::string fileName) {
    std::vector<Vector4> positions;     // 位置
    std::vector<Vector3> normals;       // 法線
    std::vector<Vector2> texCoords;     // テクスチャ座標
    std::vector<uint32_t> index;        // インデックスデータ
    std::vector<VertexData> vertices;   // 頂点データ
    std::string materialFileName;       // マテリアルファイルの名前
    std::string usemtl;                 // 使用するマテリアル名
    std::string line;                   // ファイルから読み込んだ1行を格納するもの

    // ファイルを開く
    std::ifstream file(directoryPath + "/" + fileName);
    if (!file.is_open()) {
        Log("Failed to open file: " + directoryPath + "/" + fileName, kLogLevelFlagError);
        assert(false);
    }

    // ファイルを1行ずつ読み込む
    std::string preIdentifier;
    std::string identifier;
    while (std::getline(file, line)) {
        std::istringstream s(line);

        // 前までの識別子を保存
        preIdentifier = identifier;
        // 先頭の識別子を読む
        s >> identifier;

        //==================================================
        // モデルデータ書き込み
        //==================================================

        // 前まで面情報を読み込んでいて、
        // かつ今は面情報じゃない行を読み込んでいたらモデルデータに書き込み
        if (preIdentifier == "f" && identifier != "f") {
            MaterialData materialData = LoadMaterialFile(directoryPath, materialFileName, usemtl);
            models_.back().CreateData(vertices, index, materialData);

            // 読み込んだデータを一部リセット
            vertices.clear();
            index.clear();
            usemtl.clear();
        }

        //==================================================
        // IDごとの処理
        //==================================================

        // identifierに応じた処理
        if (identifier == "o") {
            //--------- メッシュ読み込み ---------//

            // 使うマテリアル名やインデックスなどをリセット
            vertices.clear();
            index.clear();
            usemtl.clear();

        } else if (identifier == "v") {
            //--------- 頂点読み込み ---------//

            Vector4 position{};
            s >> position.x >> position.y >> position.z;
            position.w = 1.0f;
            // モデルは右手系なので左手系に変換
            position.x *= -1.0f;
            positions.push_back(position);

        } else if (identifier == "vt") {
            //--------- テクスチャ座標読み込み ---------//

            Vector2 texCoord{};
            s >> texCoord.x >> texCoord.y;
            // テクスチャ座標はY軸反転
            texCoord.y = 1.0f - texCoord.y;
            texCoords.push_back(texCoord);

        } else if (identifier == "vn") {
            //--------- 法線ベクトル読み込み ---------//

            Vector3 normal{};
            s >> normal.x >> normal.y >> normal.z;
            // モデルは右手系なので左手系に変換
            normal.x *= -1.0f;
            normals.push_back(normal);

        } else if (identifier == "f") {
            //--------- 他データ(面やマテリアル情報)読み込み ---------//

            // 前までのIDがfでなければ新しくモデルデータを追加
            if (preIdentifier != "f") {
                models_.emplace_back();
            }

            std::vector<VertexData> faceVertices;
            while (true) {
                // 読み込む頂点が無くなったら終了
                if (s.eof()) {
                    break;
                }

                std::string vertexDefinition;
                s >> vertexDefinition;

                // 頂点の要素へのindexは「位置/UV/法線」で格納されているので、分解してindexを取得する
                std::istringstream v(vertexDefinition);
                uint32_t elementIndices[3] = { 1, 1, 1 };
                for (int32_t element = 0; element < 3; ++element) {
                    std::string indexNum;
                    // 区切りでインデックスを読んでいく
                    std::getline(v, indexNum, '/');
                    // indexが空文字列の場合は、次の要素へ
                    if (indexNum.empty()) {
                        continue;
                    }
                    elementIndices[element] = std::stoi(indexNum);
                }

                // 要素へのindexから、実際の要素の値を取得して、頂点を構成する
                Vector4 position = positions[elementIndices[0] - 1];
                Vector2 texCoord = texCoords[elementIndices[1] - 1];
                Vector3 normal = normals[elementIndices[2] - 1];
                faceVertices.push_back({ position, texCoord, normal });
            }

            // 元々は逆順に格納するための部分だったけど、たぶんここもう意味ないかも
            for (size_t i = 0; i < faceVertices.size(); ++i) {
                vertices.push_back(faceVertices[i]);
            }
            
            // インデックスを設定する
            size_t indexOffset = vertices.size() - faceVertices.size();
            for (size_t i = 0; i <= faceVertices.size() - 3; ++i) {
                if (i % 2 == 0) {
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 2)));
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 1)));
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 0)));
                } else {
                    index.push_back(static_cast<uint32_t>(indexOffset + (i - 1)));
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 2)));
                    index.push_back(static_cast<uint32_t>(indexOffset + (i + 1)));
                }
            }

        } else if (identifier == "usemtl") {
            //--------- 使用するマテリアル名読み込み ---------//
            s >> usemtl;

        } else if (identifier == "mtllib") {
            //--------- マテリアルファイル名読み込み ---------//
            s >> materialFileName;
        }
    }

    // モデルデータの最後の要素のmeshがemptyなら書き込み
    if (!models_.back().isMeshExist()) {
        MaterialData materialData = LoadMaterialFile(directoryPath, materialFileName, usemtl);
        models_.back().CreateData(vertices, index, materialData);
    }
}

void Model::Draw() {
    // ワールド行列の計算
    worldMatrix_.SetSRT(
        transform_.scale,
        transform_.rotate,
        transform_.translate
    );

    // 各モデルデータの描画
    for (auto &model : models_) {
        model.Draw();
    }
}

void Model::Draw(WorldTransform &worldTransform) {
    for (auto &model : models_) {
        model.Draw(worldTransform);
    }
}

void Model::SetRenderer(Renderer *renderer) {
    for (auto &model : models_) {
        model.SetRenderer(renderer);
    }
}

} // namespace KashipanEngine