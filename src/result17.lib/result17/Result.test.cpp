#include "Result.h"

#include "Result.ops.h"
#include "Result.ostream.h"

#include <gtest/gtest.h>

using namespace result17;

enum class Codes { A, B, C };
inline auto operator<<(std::ostream& out, Codes c) -> std::ostream& {
    switch (c) {
    case Codes::A: return out << "A";
    case Codes::B: return out << "B";
    case Codes::C: return out << "C";
    }
    return out;
}

TEST(Result, okCopy) {
    auto ok = Ok<int>{};
    auto r = Result<int, Codes>{ok};

    EXPECT_TRUE(r.is_ok());
    EXPECT_FALSE(r.is_error());

    EXPECT_EQ(r, ok);
    // EXPECT_EQ(r.ok(), ok); // reference_wrapper has no ==
}

TEST(Result, errorCopy) {
    auto error = Error<Codes>{};
    auto r = Result<int, Codes>{error};

    EXPECT_FALSE(r.is_ok());
    EXPECT_TRUE(r.is_error());

    EXPECT_EQ(r, error);
    // EXPECT_EQ(r.error(), error); // reference_wrapper has no ==
}
