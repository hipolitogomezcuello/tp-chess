// Test trivial de la misión 2.1: verifica que el build, GoogleTest y ctest funcionan.
// Se puede borrar cuando existan los primeros tests reales del dominio (misión 3.1).
#include <gtest/gtest.h>

TEST(SmokeTest, BuildAndTestToolchainWork) {
    EXPECT_EQ(1 + 1, 2);
}
