#include <gtest/gtest.h>
#include <string>
#include <array>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <cmath>

static int g_argc = 0;
static char **g_argv = nullptr;

// Compare two image data buffers (read in reverse order) with tolerance; allow looser tolerance on border pixels.
// If a mismatch is found, returns false and fills out mismatch details.
static bool compare_with_tolerance(const std::string &out, const std::string &exp,
                                   std::size_t width, std::size_t height,
                                   int inner_tol, int edge_tol,
                                   std::size_t &out_x, std::size_t &out_y,
                                   int &out_a, int &out_b)
{
    if (out.size() != exp.size())
    {
        return false;
    }

    const std::size_t data_len = out.size();

    for (std::size_t rev_idx = 0; rev_idx < data_len; ++rev_idx)
    {
        // Convert reverse index to forward x,y to locate borders.
        const std::size_t fwd_idx = data_len - 1 - rev_idx;
        const std::size_t x = fwd_idx % width;
        const std::size_t y = fwd_idx / width;
        const bool is_edge = (x < 3 || y < 3 || x >= width - 3 || y >= height - 3);
        const int tol = is_edge ? edge_tol : inner_tol;

        const int a = static_cast<unsigned char>(out[rev_idx]);
        const int b = static_cast<unsigned char>(exp[rev_idx]);
        if (is_edge && edge_tol < 0)
        {
            continue; // Ignore edge mismatches entirely if edge tolerance is negative.
        }

        if (std::abs(a - b) > tol)
        {
            out_x = x;
            out_y = y;
            out_a = a;
            out_b = b;
            return false;
        }
    }

    return true;
}

// Read the last IMAGE_DATA_LEN bytes of the file in reverse order to avoid parsing the header.
static std::string read_image_data_reverse(const std::string &path, std::size_t image_data_len)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to open file: " + path);
    }

    file.seekg(0, std::ios::end);
    const auto file_size = static_cast<std::size_t>(file.tellg());
    if (file_size < image_data_len)
    {
        throw std::runtime_error("File too small to contain expected image data: " + path);
    }

    std::string data;
    data.reserve(image_data_len);

    // Start at the end of the file and read backwards image_data_len bytes.
    for (std::size_t i = 0; i < image_data_len; ++i)
    {
        const auto pos = static_cast<std::streamoff>(file_size - 1 - i);
        file.seekg(pos, std::ios::beg);
        char c = 0;
        file.read(&c, 1);
        data.push_back(c);
    }

    return data;
}

TEST(HelloBinaryTest, OutputContainsHelloWorld)
{
    ASSERT_GE(g_argc, 2) << "Expected binary path as first argument to test.";
    std::string exePath = std::string(g_argv[1]);

    // Known image dimensions: 720x576 => 414720 bytes of data.
    constexpr std::size_t IMAGE_WIDTH = 720u;
    constexpr std::size_t IMAGE_HEIGHT = 576u;
    constexpr std::size_t IMAGE_DATA_LEN = IMAGE_WIDTH * IMAGE_HEIGHT;

    // Delete output files if they already exist
    std::filesystem::remove("image/boats-gradient-hedge.pgm");
    std::filesystem::remove("image/boats-gradient-vedge.pgm");
    std::filesystem::remove("image/boats-gradient-magedge.pgm");

#ifdef _WIN32
    std::string cmd = "\"" + exePath + "\" image\\boats.pgm image\\boats-gradient";
#else
    std::string cmd = exePath + " image/boats.pgm image/boats-gradient";
#endif

    // Run the filter command
    int result = std::system(cmd.c_str());
    EXPECT_EQ(result, 0) << "Filter command failed with code: " << result;

    // Check if output files exist
    EXPECT_TRUE(std::filesystem::exists("image/boats-gradient-hedge.pgm")) << "Horizontal edge file not created";
    EXPECT_TRUE(std::filesystem::exists("image/boats-gradient-vedge.pgm")) << "Vertical edge file not created";
    EXPECT_TRUE(std::filesystem::exists("image/boats-gradient-magedge.pgm")) << "Magnitude edge file not created";

    // Compare generated images to expected images, ignoring headers by reading the data tail in reverse.
    const auto hedge_out = read_image_data_reverse("image/boats-gradient-hedge.pgm", IMAGE_DATA_LEN);
    const auto hedge_exp = read_image_data_reverse("tests/boats-gradient-hedge.pgm", IMAGE_DATA_LEN);
    {
        std::size_t mx = 0, my = 0;
        int a = 0, b = 0;
        const bool ok = compare_with_tolerance(hedge_out, hedge_exp, IMAGE_WIDTH, IMAGE_HEIGHT, /*inner_tol=*/1, /*edge_tol=*/-1, mx, my, a, b);
        EXPECT_TRUE(ok) << "Horizontal edge image data mismatch at (" << mx << "," << my << ") got=" << a << " exp=" << b;
    }

    const auto vedge_out = read_image_data_reverse("image/boats-gradient-vedge.pgm", IMAGE_DATA_LEN);
    const auto vedge_exp = read_image_data_reverse("tests/boats-gradient-vedge.pgm", IMAGE_DATA_LEN);
    {
        std::size_t mx = 0, my = 0;
        int a = 0, b = 0;
        const bool ok = compare_with_tolerance(vedge_out, vedge_exp, IMAGE_WIDTH, IMAGE_HEIGHT, /*inner_tol=*/1, /*edge_tol=*/-1, mx, my, a, b);
        EXPECT_TRUE(ok) << "Vertical edge image data mismatch at (" << mx << "," << my << ") got=" << a << " exp=" << b;
    }

    const auto mag_out = read_image_data_reverse("image/boats-gradient-magedge.pgm", IMAGE_DATA_LEN);
    const auto mag_exp = read_image_data_reverse("tests/boats-gradient-magedge.pgm", IMAGE_DATA_LEN);
    {
        std::size_t mx = 0, my = 0;
        int a = 0, b = 0;
        const bool ok = compare_with_tolerance(mag_out, mag_exp, IMAGE_WIDTH, IMAGE_HEIGHT, /*inner_tol=*/1, /*edge_tol=*/-1, mx, my, a, b);
        EXPECT_TRUE(ok) << "Magnitude edge image data mismatch at (" << mx << "," << my << ") got=" << a << " exp=" << b;
    }
}

int main(int argc, char **argv)
{
    // Save argv for tests
    g_argc = argc;
    g_argv = argv;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
