#include "ShipWeapon.h"
#include "../Mx/Log/MxLog.h"
#include "../Mx/Time/MxTime.h"
#include "../Mx/GameObject/MxGameObject.h"
#include "Bullet.h"
#include "../Mx/Graphics/Mesh/MxMeshUtils.h"
#include "../Mx/Resource/MxResourceLoader.h"
#include "../Mx/Graphics/Texture/MxTexture.h"
#include "../Mx/Component/Renderer/MxRenderer.h"
#include "../Mx/Graphics/MxGraphics.h"
#include "../Mx/Component/MeshFilter/MxMeshFilter.h"

namespace Scripts {
    MX_IMPLEMENT_RTTI(ShipWeapon, Script);

    void ShipWeapon::attack() {
        float attackGap = 1.0f / mAttackSpeed;
        if (mAttackGap >= attackGap) {
            fire();
            mAttackGap = 0.0f;
        }
    }

    void ShipWeapon::setAttackSpeed(float _speed) {
        if (_speed >= 0.0f) {
            mAttackSpeed = _speed;
            mAttackGap = 1.0f / mAttackSpeed;
        }
    }

    void ShipWeapon::awake() {
        setAttackSpeed(mAttackSpeed);
    }

    void ShipWeapon::init() {
        auto[vertices, indices] = MeshUtils::Quad(Vector2f(1.5f, 1.5f));
        std::vector<Mesh::UV2DType> uvs = {
           {1.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f},{1.0f,0.0f}
        };

        auto mesh = std::make_shared<Mesh>();
        mesh->setPositions(std::move(vertices));
        mesh->setIndices(std::move(indices), MeshTopology::Triangles_List, 0);
        mesh->setUVs(UVChannel::UV0, std::move(uvs));
        mesh->recalculateNormals();
        mesh->uploadMeshData(false);

        auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("Resource/Textures/fx_lazer_blue_dff.png");
        auto material = std::make_shared<Material>(Graphics::Get()->findShader("Standard"));
        material->setTexture("diffuseTex", diffuseTex);
        material->setRenderType(RenderType::Transparent);

        mBulletMat = std::move(material);
        mBulletMesh = std::move(mesh);
    }

    void ShipWeapon::update() {
        mAttackGap += Time::DeltaTime();
    }

    void ShipWeapon::fire() {
        auto newBullet = GameObject::Create("Bullet_" + std::to_string(++mFireCount), "Bullet");
        newBullet->addComponent<Bullet>();

        auto meshFilter = newBullet->addComponent<MeshFilter>();
        meshFilter->setMesh(mBulletMesh);

        auto renderer = newBullet->addComponent<Renderer>();
        renderer->setMaterial(mBulletMat);

        newBullet->transform().rotate(Vector3f::Right, Math::Radians(-90.0f), Space::World);
        newBullet->transform().setPosition(transform()->getPosition());
    }


}
