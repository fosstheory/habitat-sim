#include <Corrade/TestSuite/Compare/Numeric.h>
#include <Corrade/TestSuite/Tester.h>

#include <Corrade/Utility/Directory.h>
#include <Magnum/EigenIntegration/GeometryIntegration.h>
#include <Magnum/EigenIntegration/Integration.h>
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector3.h>

#include "configure.h"
#include "esp/scene/ReplicaSemanticScene.h"
#include "esp/scene/SemanticScene.h"
#include "esp/sim/Simulator.h"

#include "esp/assets/GenericInstanceMeshData.h"

namespace Cr = Corrade;
namespace Mn = Magnum;

namespace {

const std::string replicaRoom0 =
    Cr::Utility::Directory::join(SCENE_DATASETS,
                                 "replica_dataset/room_0/habitat");

struct ReplicaSceneTest : Cr::TestSuite::Tester {
  explicit ReplicaSceneTest();

  void testSemanticSceneOBB();

  void testSemanticSceneLoading();
};

ReplicaSceneTest::ReplicaSceneTest() {
  addTests({&ReplicaSceneTest::testSemanticSceneOBB,
            &ReplicaSceneTest::testSemanticSceneLoading});
}

void ReplicaSceneTest::testSemanticSceneOBB() {
  if (!Cr::Utility::Directory::exists(replicaRoom0)) {
    CORRADE_SKIP("Replica dataset not found at '" + replicaRoom0 +
                 "'\nSkipping test");
  }

  esp::scene::SemanticScene scene;
  CORRADE_VERIFY(esp::scene::SemanticScene::loadReplicaHouse(
      Cr::Utility::Directory::join(replicaRoom0, "info_semantic.json"), scene));

  esp::assets::GenericInstanceMeshData mesh;
  CORRADE_VERIFY(mesh.loadPLY(
      Cr::Utility::Directory::join(replicaRoom0, "mesh_semantic.ply")));

  const auto& vbo = mesh.getVertexBufferObjectCPU();
  const auto& objectIds = mesh.getObjectIdsBufferObjectCPU();
  const auto& ibo = mesh.getIndexBufferObjectCPU();

  CORRADE_VERIFY(objectIds.size() == ibo.size());
  for (const auto& obj : scene.objects()) {
    if (obj == nullptr)
      continue;

    const auto& stringId = obj->id();
    const int id = std::stoi(stringId.substr(stringId.find_last_of("_") + 1));
    CORRADE_ITERATION(stringId);

    for (uint64_t fid = 0; fid < ibo.size(); fid += 6) {
      CORRADE_ITERATION(fid);
      if (objectIds[fid] == id) {
        esp::vec3f quadCenter = esp::vec3f::Zero();
        // Mesh was converted from quads to tris
        for (int i = 0; i < 6; ++i) {
          quadCenter += vbo[ibo[fid + i]] / 6;
        }

        CORRADE_VERIFY(obj->obb().contains(quadCenter, 5e-2));
      }
    }
  }
}

void ReplicaSceneTest::testSemanticSceneLoading() {
  if (!Cr::Utility::Directory::exists(replicaRoom0)) {
    CORRADE_SKIP("Replica dataset not found at '" + replicaRoom0 +
                 "'\nSkipping test");
  }

  esp::sim::SimulatorConfiguration cfg;
  cfg.scene.id =
      Cr::Utility::Directory::join(replicaRoom0, "mesh_semantic.ply");

  esp::sim::Simulator sim{cfg};

  const auto& scene = sim.getSemanticScene();
  CORRADE_VERIFY(scene != nullptr);
  CORRADE_COMPARE(scene->objects().size(), 94);

  CORRADE_VERIFY(scene->objects()[12] != nullptr);
  CORRADE_COMPARE(scene->objects()[12]->id(), "_12");

  CORRADE_VERIFY(scene->objects()[12]->category() != nullptr);

  CORRADE_COMPARE(scene->objects()[12]->category()->index(), 13);
  CORRADE_COMPARE(scene->objects()[12]->category()->name(), "book");
}

}  // namespace

CORRADE_TEST_MAIN(ReplicaSceneTest)
