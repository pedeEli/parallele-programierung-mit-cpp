#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <cstdlib>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <vector>

template<typename T>
class Ringbuffer {
  private:
    mutable std::mutex m_mutex;
    std::condition_variable m_ready_for_reading;
    std::condition_variable m_ready_for_writing;
    const std::size_t m_capacity;
    std::vector<T> m_buffer;
    std::size_t m_filled = 0;
    std::size_t m_read_index = 0;
    std::size_t m_write_index = 0;
    bool m_finished = false;

  public:
    Ringbuffer(std::size_t capacity) : m_capacity(capacity), m_buffer(capacity) {} 

    bool write(T item) {
      std::unique_lock guard(m_mutex);
      while (!m_finished && m_filled == m_capacity) {
        m_ready_for_writing.wait(guard);
      }

      if (m_finished) {
        return false;
      }

      m_buffer[m_write_index] = std::move(item);
      m_write_index = (m_write_index + 1) & m_capacity;
      m_filled++;
      m_ready_for_reading.notify_one();
      return true;
    }

    std::optional<T> read() {
      std::unique_lock guard(m_mutex);
      while (!m_finished && m_filled == 0) {
        m_ready_for_reading.wait(guard);
      }

      if (m_filled == 0) {
        return {};
      }

      auto item = std::move(m_buffer[m_read_index]);
      m_read_index = (m_read_index + 1) & m_capacity;
      m_filled--;
      m_ready_for_writing.notify_one();
      return item;
    }

    void done() {
      std::lock_guard lock(m_mutex);
      m_finished = true;
      m_ready_for_reading.notify_all();
      m_ready_for_writing.notify_all();
    }

    bool is_finished() {
      std::lock_guard lock(m_mutex);
      return m_filled == 0 && m_finished;
    }
};


#endif