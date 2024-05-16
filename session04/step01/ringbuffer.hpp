#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <cstdlib>
#include <mutex>
#include <optional>
#include <vector>

template<typename T>
class Ringbuffer {
  private:
    mutable std::mutex m_mutex;
    const std::size_t m_capacity;
    std::vector<T> m_buffer;
    std::size_t m_filled = 0;
    std::size_t m_read_index = 0;
    std::size_t m_write_index = 0;
    bool m_finished = false;

  public:
    Ringbuffer(std::size_t capacity) : m_capacity(capacity), m_buffer(capacity) {} 

    bool write(T item) {
      std::lock_guard lock(m_mutex);
      if (m_finished || m_filled == m_capacity) {
        return false;
      }

      m_buffer[m_write_index] = std::move(item);
      m_write_index = (m_write_index + 1) & m_capacity;
      m_filled++;
      return true;
    }

    std::optional<T> read() {
      std::lock_guard lock(m_mutex);
      if (m_filled == 0) {
        return {};
      }

      auto item = std::move(m_buffer[m_read_index]);
      m_read_index = (m_read_index + 1) & m_capacity;
      m_filled--;
      return item;
    }

    void done() {
      std::lock_guard lock(m_mutex);
      m_finished = true;
    }

    bool is_finished() {
      std::lock_guard lock(m_mutex);
      return m_filled == 0 && m_finished;
    }
};


#endif