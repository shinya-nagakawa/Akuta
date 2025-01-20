#include "../GLLibrary.h"
#ifdef USE_PHYSX_LIB
#include "CPhysX.h"
#include "CMatrix.h"
#include "CFPS.h"

CPhysX* CPhysX::mp_instance=nullptr;
CPhysX::CPhysX()
{
    Init();
}

CPhysX::~CPhysX()
{
    PxCloseExtensions();
    m_pScene->release();
    m_pDispatcher->release();
    m_pPhysics->release();
   
    m_pFoundation->release();
}

bool CPhysX::Init()
{
    
        
        
        
    if (!LoadLibrary("PhysX_64.dll")) {
        printf("%x\n",GetLastError());
    }
        LoadLibrary("PhysXCommon_64.dll");
        LoadLibrary("PhysXCooking_64.dll");
        LoadLibrary("PhysXFoundation_64.dll");
    // Foundationのインスタンス化
    if (!(m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback))) {
        return false;
    }
    // Physicsのインスタンス化
    if (!(m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, physx::PxTolerancesScale(), true))) {
        return false;
    }
    
    // 処理に使うスレッドを指定する
    m_pDispatcher = physx::PxDefaultCpuDispatcherCreate(8);
    // 空間の設定
    physx::PxSceneDesc scene_desc(m_pPhysics->getTolerancesScale());
    scene_desc.gravity = physx::PxVec3(0, -9, 0);
    scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
    scene_desc.cpuDispatcher = m_pDispatcher;
    // 空間のインスタンス化
    m_pScene = m_pPhysics->createScene(scene_desc);
    // PVDの表示設定
    physx::PxPvdSceneClient* pvd_client;
    if (pvd_client = m_pScene->getScenePvdClient()) {
        pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

void CPhysX::CreateInstance()
{
    if (mp_instance) return;
    mp_instance = new CPhysX();
}
void CPhysX::ClearInstance() {
    delete mp_instance;
    mp_instance = nullptr;
}
void CPhysX::Update()
{
    // シミュレーション速度を指定する
    m_pScene->simulate(CFPS::GetDeltaTime());
    // PhysXの処理が終わるまで待つ
    m_pScene->fetchResults(true);
}
CPhysX* CPhysX::GetInstance()
{
    return mp_instance;
}


physx::PxMat44 ToPxMat44(const CMatrix& mat) {
    //倒置
    return physx::PxMat44(
        physx::PxVec4(mat.m00, mat.m10, mat.m20, mat.m30),
        physx::PxVec4(mat.m01, mat.m11, mat.m21, mat.m31),
        physx::PxVec4(mat.m02, mat.m12, mat.m22, mat.m32),
        physx::PxVec4(mat.m03, mat.m13, mat.m23, mat.m33)
    );
}
physx::PxVec3 ToPxVec3(const CVector3D& vec3) {
    return physx::PxVec3(vec3.x, vec3.y, vec3.z);
}

physx::PxVec4 ToPxVec4(const CVector4D& vec4) {
    return physx::PxVec4(vec4.x, vec4.y, vec4.z, vec4.w);
}

physx::PxQuat ToPxQuat(const CQuaternion& quat) {
    return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
}

physx::PxTransform ToPxTransform(const CVector3D& position, const CVector3D& rotation) {
    return physx::PxTransform(ToPxVec3(position), ToPxQuat(CQuaternion::Euler(rotation)));
    }
physx::PxTransform ToPxTransform(CVector3D& position, CQuaternion& quaternion) {
    return physx::PxTransform(ToPxVec3(position), ToPxQuat(quaternion));
}
CMatrix ToMatrix(const physx::PxMat44& px_mat44) {
    //倒置
    return CMatrix(
        px_mat44.column0.x, px_mat44.column1.x, px_mat44.column2.x, px_mat44.column3.x,
        px_mat44.column0.y, px_mat44.column1.y, px_mat44.column2.y, px_mat44.column3.y,
        px_mat44.column0.z, px_mat44.column1.z, px_mat44.column2.z, px_mat44.column3.z,
        px_mat44.column0.w, px_mat44.column1.w, px_mat44.column2.w, px_mat44.column3.w
    );
}

CVector3D ToVector3(const physx::PxVec3& px_vec3) {
    return CVector3D(px_vec3.x, px_vec3.y, px_vec3.z);
}

CVector4D ToVector4(const physx::PxVec4& px_vec4) {
    return CVector4D(px_vec4.x, px_vec4.y, px_vec4.z, px_vec4.w);
}

CQuaternion ToQuaternion(const physx::PxQuat& px_quat) {
    return CQuaternion(px_quat.x, px_quat.y, px_quat.z, px_quat.w);
}
#endif