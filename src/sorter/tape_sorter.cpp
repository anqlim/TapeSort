#include "tape_sorter.h"

namespace Sorting {

    TapeSorter::TapeSorter(const Configuration::Config& config, const Tape::TapeFactory& factory)
                           : config_(config), factory_(factory) {}

    void TapeSorter::sort(Tape::ITape& input, Tape::ITape& output) {
        std::filesystem::create_directories("tmp");
        size_t chunk_count = split(input);
        merge(output, chunk_count);
        std::filesystem::remove_all("tmp");
    }

    size_t TapeSorter::split(Tape::ITape& input) {
        size_t n = input.size(), chunk_count = 0;
        const size_t limit = config_.memoryLimitBytes() / sizeof (int32_t);
        while (chunk_count < (n + limit - 1) / limit) {
            size_t current_chunk_size = std::min(limit, n - chunk_count * limit);
            std::vector<int32_t> buf(current_chunk_size);
            for (size_t i = 0; i < current_chunk_size; i++) {
                buf[i] = input.read();
                input.moveRight();
            }
            std::sort(buf.begin(), buf.end());

            auto current_chunk = factory_.createTape("tmp/chunk_" + std::to_string(chunk_count), current_chunk_size);
            for (size_t i = 0; i < current_chunk_size; i++) {
                current_chunk->write(buf[i]);
                current_chunk->moveRight();
            }

            chunk_count++;
        }
        return chunk_count;
    }

    void TapeSorter::merge(Tape::ITape& output, size_t chunk_count, size_t pass) {
        std::priority_queue<std::pair<int32_t, size_t>, std::vector<std::pair<int32_t, size_t>>, std::greater<>> queue;
        const size_t k = config_.memoryLimitBytes() / (sizeof(int32_t) + sizeof(size_t) + sizeof(std::unique_ptr<Tape::ITape>));
        if (chunk_count <= k) {
            std::vector<std::unique_ptr<Tape::ITape>> chunks(chunk_count);
            for (size_t i = 0; i < chunk_count; i++) {
                chunks[i] = factory_.openTape("tmp/chunk_" + std::to_string(i));
                queue.emplace(chunks[i]->read(), i);
                chunks[i]->moveRight();
            }

            while (!queue.empty()) {
                auto temp = queue.top();
                queue.pop();
                output.write(temp.first);
                output.moveRight();
                if (chunks[temp.second]->position() < chunks[temp.second]->size()) {
                    queue.emplace(chunks[temp.second]->read(), temp.second);
                    chunks[temp.second]->moveRight();
                }
            }
            return;
        }

        size_t count = 0;
        while (count * k < chunk_count) {
            std::vector<std::unique_ptr<Tape::ITape>> chunks(std::min(k, chunk_count - count * k));
            size_t total_size = 0;
            for (size_t i = 0; i < chunks.size(); i++) {
                chunks[i] = factory_.openTape("tmp/chunk_" + std::to_string(count * k + i));
                total_size += chunks[i]->size();
                queue.emplace(chunks[i]->read(), i);
                chunks[i]->moveRight();
            }
            auto tmp_output = factory_.createTape("tmp/tmp_" + std::to_string(count), total_size);
            while (!queue.empty()) {
                auto temp = queue.top();
                queue.pop();
                tmp_output->write(temp.first);
                tmp_output->moveRight();
                if (chunks[temp.second]->position() < chunks[temp.second]->size()) {
                    queue.emplace(chunks[temp.second]->read(), temp.second);
                    chunks[temp.second]->moveRight();
                }
            }
            count++;
        }

        for (size_t i = 0; i < chunk_count; i++)
            std::filesystem::remove("tmp/chunk_" + std::to_string(i));

        for (size_t i = 0; i < count; i++)
            std::filesystem::rename(
                    "tmp/tmp_" + std::to_string(i),
                    "tmp/chunk_" + std::to_string(i)
            );

        merge(output, count, pass + 1);
    }

}