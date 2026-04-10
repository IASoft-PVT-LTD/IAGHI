// IAGHI: IA Graphics Hardware Interface
// Copyright (C) 2026 IAS (ias@iasoft.dev)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <backends/vulkan/backend.hpp>

namespace ghi
{
  class VulkanDevice;

  class VulkanBuffer
  {
public:
    struct Data
    {
      VkBuffer handle{};
      VmaAllocation allocation{};
      VmaAllocationInfo alloc_info{};
    };

    explicit VulkanBuffer(VulkanDevice& device_ref) : m_device_ref(device_ref)
    {
    }

    static auto create(VulkanDevice& device, u64 size, VkBufferUsageFlags usage, bool host_visible,
                       const char *debug_name = "<not_set>") -> Result<VulkanBuffer>;

    auto destroy() -> void;

    auto get_data(u32 index) -> Data&
    {
      return m_data[index];
    }

    [[nodiscard]] auto get_data_count() const -> u64
    {
      return m_data_count;
    }

    auto get_device() -> VulkanDevice&
    {
      return m_device_ref;
    }

private:
    Data m_data[NUM_FRAMES_BUFFERED]; // [IATODO]: Memory inefficient yeah but for now this'll do

    u64 m_size{};
    u32 m_data_count{};
    VulkanDevice& m_device_ref;

    friend class VulkanBackend;
    friend class VulkanDeviceLocalBuffer;
    friend class VulkanHostVisibleBuffer;
  };
} // namespace ghi