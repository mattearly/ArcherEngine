#include "pch.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace CoreTests {

static AA::AncientArcher instance;

TEST_CLASS(CoreTests) {
public:
  TEST_METHOD(A_BaseWindow) {
    instance.Init();
    instance.SetWindowTitle("A_BaseWindow");
    instance.AddToImGuiUpdate([](){
      ImGui::Begin("Base Run Test");
      ImGui::Text("Click \"ok\" to continue");
      bool result = ImGui::Button("ok");
      if (result) { instance.Shutdown(); };
      ImGui::End();
    });
    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
  }

  TEST_METHOD(B_TheNoInitCase) {
    int result = instance.Run();
    Assert::AreEqual(result, -4);
  }

  //TEST_METHOD(C_PHYSICS_TEST) {
  //  AA::Init();
  //  // add random physx objects
  //  for (unsigned int i = 0; i < 32; i+=2)
  //    AA::AddBoxCollider(physx::PxVec3(0, i, 0), physx::PxVec3(1, 1, 1));
  //  AA::SimulateWorldPhysics(true);
  //  AA::AddCamera();
  //  int result = AA::Run();
  //  // look at debugger...
  //  Assert::AreEqual(result, 0);
  //}
};
}
