#include <gtest/gtest.h>
#include "tape/file_tape.h"
#include "config/config.h"
#include <fstream>

using namespace Tape;

namespace Testing {

    static Configuration::Config makeConfig() {
        return Configuration::Config("nonexistent.json");
    }

    static void createBinaryFile(const std::string &path, const std::vector<int32_t> &data) {
        std::ofstream f(path, std::ios::binary);
        f.write(reinterpret_cast<const char *>(data.data()), data.size() * sizeof(int32_t));
    }

    // Конфиг
    TEST(FileTapeTest, ConfigWithDelays) {
        std::ofstream cfg("test_config.json");
        cfg << R"({"read_delay_ms":1,"write_delay_ms":1,"shift_delay_ms":1,"rewind_delay_ms":1})";
        cfg.close();

        Configuration::Config config("test_config.json");
        EXPECT_EQ(config.delays().read_delay_ms, 1);
        EXPECT_EQ(config.delays().write_delay_ms, 1);
    }

    // Открытие
    TEST(FileTapeTest, OpenExisting) {
        createBinaryFile("test_open.bin", {1, 2, 3});
        EXPECT_NO_THROW(FileTape("test_open.bin", makeConfig()));
    }

    TEST(FileTapeTest, OpenNonExistent) {
        EXPECT_THROW(FileTape("no_such_file.bin", makeConfig()), std::runtime_error);
    }

    TEST(FileTapeTest, CreateNew) {
        EXPECT_NO_THROW(FileTape("test_create.bin", 5, makeConfig()));
    }

    TEST(FileTapeTest, SizeFromFile) {
        createBinaryFile("test_size.bin", {1, 2, 3, 4});
        FileTape tape("test_size.bin", makeConfig());
        EXPECT_EQ(tape.size(), 4);
    }

    TEST(FileTapeTest, SizeFromCount) {
        FileTape tape("test_size2.bin", 7, makeConfig());
        EXPECT_EQ(tape.size(), 7);
    }

    // Чтение/запись
    TEST(FileTapeTest, ReadFirst) {
        createBinaryFile("test_read.bin", {42, 1, 2});
        FileTape tape("test_read.bin", makeConfig());
        EXPECT_EQ(tape.read(), 42);
    }

    TEST(FileTapeTest, WriteAndRead) {
        FileTape tape("test_write.bin", 3, makeConfig());
        tape.write(99);
        EXPECT_EQ(tape.read(), 99);
    }

    TEST(FileTapeTest, ReadDoesNotMovePosition) {
        createBinaryFile("test_read_pos.bin", {1, 2, 3});
        FileTape tape("test_read_pos.bin", makeConfig());
        tape.read();
        tape.read();
        EXPECT_EQ(tape.read(), 1);
    }

    TEST(FileTapeTest, WriteDoesNotMovePosition) {
        FileTape tape("test_write_pos.bin", 3, makeConfig());
        tape.write(5);
        tape.write(10);
        EXPECT_EQ(tape.read(), 10);
    }

    // Перемещение
    TEST(FileTapeTest, MoveRightAndRead) {
        createBinaryFile("test_move.bin", {1, 2, 3});
        FileTape tape("test_move.bin", makeConfig());
        tape.moveRight();
        EXPECT_EQ(tape.read(), 2);
    }

    TEST(FileTapeTest, MoveLeftAndRead) {
        createBinaryFile("test_moveleft.bin", {1, 2, 3});
        FileTape tape("test_moveleft.bin", makeConfig());
        tape.moveRight();
        tape.moveRight();
        tape.moveLeft();
        EXPECT_EQ(tape.read(), 2);
    }

    TEST(FileTapeTest, MoveLeftAtBeginning) {
        createBinaryFile("test_moveleft_begin.bin", {1, 2});
        FileTape tape("test_moveleft_begin.bin", makeConfig());
        EXPECT_THROW(tape.moveLeft(), std::runtime_error);
    }

    TEST(FileTapeTest, MoveRightAtEnd) {
        FileTape tape("test_moveright_end.bin", 2, makeConfig());
        tape.moveRight(); // позиция 1
        tape.moveRight(); // позиция 2 — за последним
        EXPECT_THROW(tape.moveRight(), std::runtime_error);
    }

    // Перемотка
    TEST(FileTapeTest, RewindResetsPosition) {
        createBinaryFile("test_rewind.bin", {1, 2, 3});
        FileTape tape("test_rewind.bin", makeConfig());
        tape.moveRight();
        tape.moveRight();
        tape.rewind();
        EXPECT_EQ(tape.read(), 1);
    }
}