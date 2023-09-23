#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>

#include "fdupl.h"
#include "hasher.h"

namespace {

TEST(Fdupl_empty_dir, Test_should_return_empty_result)
{
    std::vector<std::string> dirs{"fixtures/emptydir"};
    lib::Fdupl fdupl(dirs);

    auto result = fdupl.Start().GetResult();

    EXPECT_EQ(result.size(), 0);
}

TEST(Fdupl_process_dir, Test_should_find_two_same_files)
{
    std::vector<std::string> dirs{"fixtures/2samefiles"};
    lib::Fdupl fdupl(dirs);
    auto result = fdupl.Start().GetResult();

    EXPECT_EQ(result.size(), 1);     // should be one set of same files
    EXPECT_EQ(result[0].size(), 2);  // should be two same files in a set
}

TEST(Fdupl_process_dir, Test_should_find_two_same_files_in_folder_of_four)
{
    std::vector<std::string> dirs{"fixtures/4files2same"};
    lib::Fdupl fdupl(dirs);
    auto result = fdupl.Start().GetResult();

    EXPECT_EQ(result.size(), 1);     // should be one set of same files
    EXPECT_EQ(result[0].size(), 2);  // should be two same files in a set
}

TEST(Fdupl_process_dir, Test_should_find_four_same_files_in_folder_recursevly)
{
    std::vector<std::string> dirs{"fixtures/recur_dirs"};
    lib::Fdupl fdupl(dirs);
    auto result = fdupl.Start().GetResult();

    EXPECT_EQ(result.size(), 1);     // should be one set of same files
    EXPECT_EQ(result[0].size(), 4);  // should be four same files in a set
}

TEST(Fdupl_process_dir, Test_should_find_three_same_empty_files)
{
    std::vector<std::string> dirs{"fixtures/3emptyfiles"};
    lib::Fdupl fdupl(dirs);
    auto result = fdupl.Start().GetResult();

    EXPECT_EQ(result.size(), 1);     // should be one set of same files
    EXPECT_EQ(result[0].size(), 3);  // should be three same files in a set
}

TEST(Fdupl_process_dir, Test_should_find_two_sets_in_one_folder)
{
    std::vector<std::string> dirs{"fixtures/smallfiles"};
    lib::Fdupl fdupl(dirs);
    auto result = fdupl.Start().GetResult();

    EXPECT_EQ(result.size(), 2);  // should be two sets of same files
}

TEST(Fdupl_process_dir, Test_should_find_four_sets_in_twopassed_folder)
{
    std::vector<std::string> dirs{"fixtures/smallfiles", "fixtures/2samefiles"};
    lib::Fdupl fdupl(dirs);
    auto result = fdupl.Start().GetResult();

    EXPECT_EQ(result.size(), 4);  // should be four sets of same files
}

TEST(Fdupl_process_dir, Test_should_find_eight_sets_of_files)
{
    std::vector<std::string> dirs{"fixtures"};
    lib::Fdupl fdupl(dirs);
    auto result = fdupl.Start().GetResult();

    // for (auto& set : result)
    // {
    //     for (auto& file : set)
    //     {
    //         std::cout << file << std::endl;
    //     }
    //     std::cout << std::endl;
    // }

    EXPECT_EQ(result.size(), 8);  // should be six set of same files including same files in different folders
}

}  // namespace