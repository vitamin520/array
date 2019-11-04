#include "array.h"
#include "test.h"
#include "lifetime.h"

namespace array {

typedef dense_array<int, 3, stack_allocator<int, 32>> dense3d_int_stack_array;

TEST(stack_array) {
  dense3d_int_stack_array stack_array({4, 3, 2});
  for_all_indices(stack_array.shape(), [&](int x, int y, int c) {
    stack_array(x, y, c) = x;
  });

  dense3d_int_stack_array copy_array(stack_array);
  for_all_indices(copy_array.shape(), [&](int x, int y, int c) {
    ASSERT_EQ(copy_array(x, y, c), x);
  });

  dense3d_int_stack_array assign_array;
  assign_array = stack_array;
  for_all_indices(assign_array.shape(), [&](int x, int y, int c) {
    ASSERT_EQ(assign_array(x, y, c), x);
  });

  dense3d_int_stack_array move_array(std::move(stack_array));
  for_all_indices(move_array.shape(), [&](int x, int y, int c) {
    ASSERT_EQ(move_array(x, y, c), x);
  });

  dense3d_int_stack_array move_assign;
  move_assign = std::move(assign_array);
  for_all_indices(move_assign.shape(), [&](int x, int y, int c) {
    ASSERT_EQ(move_assign(x, y, c), x);
  });
}

TEST(stack_array_bad_alloc) {
  try {
    // This array is too big for our stack allocator.
    dense3d_int_stack_array stack_array({4, 3, 5});
    ASSERT(false);
  } catch (std::bad_alloc) {
    // This is success.
  }
}

TEST(stack_array_make_copy) {
  dense3d_int_stack_array src({4, 3, 2});

  dense3d_int_stack_array copy = make_dense_copy(src);
  ASSERT(src == copy);
}

TEST(stack_array_move_constructor) {
  typedef dense_array<lifetime_counter, 3, stack_allocator<lifetime_counter, 32>> lifetime_stack_array;

  lifetime_stack_array stack_array({4, 3, 2});

  lifetime_counter::reset();
  lifetime_stack_array move_array(std::move(stack_array));

  ASSERT_EQ(lifetime_counter::default_constructs, 0);
  ASSERT_EQ(lifetime_counter::copy_constructs, 0);
  ASSERT_EQ(lifetime_counter::move_constructs, move_array.size());
}

TEST(stack_array_move_assignment) {
  typedef dense_array<lifetime_counter, 3, stack_allocator<lifetime_counter, 32>> lifetime_stack_array;

  lifetime_stack_array stack_array({4, 3, 2});

  lifetime_counter::reset();
  lifetime_stack_array move_assign;
  move_assign = std::move(stack_array);

  ASSERT_EQ(lifetime_counter::default_constructs, 0);
  ASSERT_EQ(lifetime_counter::copy_constructs, 0);
  // TODO: Is it OK that this assignment uses move constructions instead of move assignments?
  ASSERT_EQ(lifetime_counter::moves(), move_assign.size());
}

}  // namespace array
