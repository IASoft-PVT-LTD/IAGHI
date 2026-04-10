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

#include <backends/vulkan/buffer.hpp>
#include <backends/vulkan/device.hpp>

namespace ghi
{
  auto VulkanBuffer::create(VulkanDevice &device, u64 size, VkBufferUsageFlags usage, bool host_visible,
                            const char *debug_name) -> Result<VulkanBuffer>
  {
    VulkanBuffer result{device};

    const auto allocator = device.get_allocator();

    result.m_size = size;

    result.m_data_count = ((usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) || (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
                              ? device.get_swapchain().get_backbuffer_image_count()
                              : 1;

    VkBufferCreateInfo buffer_info{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    };

    VmaAllocationCreateInfo alloc_create_info{
        .flags = host_visible
                     ? static_cast<VmaAllocationCreateFlags>(VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                                                             VMA_ALLOCATION_CREATE_MAPPED_BIT)
                     : static_cast<VmaAllocationCreateFlags>(0),
        .usage = host_visible ? VMA_MEMORY_USAGE_AUTO : VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
    };

    for (u32 i = 0; i < result.m_data_count; i++)
    {
      VK_CALL(vmaCreateBuffer(allocator, &buffer_info, &alloc_create_info, &result.m_data[i].handle,
                              &result.m_data[i].allocation, &result.m_data[i].alloc_info),
              "Creating buffer");
      vmaSetAllocationName(allocator, result.m_data[i].allocation, debug_name);
    }

    return result;
  }

  auto VulkanBuffer::destroy() -> void
  {
    for (u32 i = 0; i < m_data_count; i++)
    {
      vmaDestroyBuffer(m_device_ref.get_allocator(), m_data[i].handle, m_data[i].allocation);
      m_data[i].handle = VK_NULL_HANDLE;
    }
  }
} // namespace ghi