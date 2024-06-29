/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL
v2. You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include "common/config.h"

/**
 * Replacer is an abstract class that tracks page usage.
 */
class Replacer {
public:
  Replacer() = default;
  virtual ~Replacer() = default;

  /**
   * Remove the victim frame as defined by the replacement policy.
   * @param[out] frame_id id of frame that was removed, nullptr if no victim was
   * found
   * @return true if a victim frame was found, false otherwise
   */
  /* @description: 根据替换策略定义的victim page, 移除victim page
   * @param frame_id_t* frame_id
   * @return 如果victim page被找到就返回true, 否则为false
   */
  virtual bool victim(frame_id_t *frame_id) = 0;

  /**
   * Pins a frame, indicating that it should not be victimized until it is
   * unpinned.
   * @param frame_id the id of the frame to pin
   */
  /*@description: 固定一个frame, 表明这个frame不可以成为victim page，
   * 直到对这个frame调用unpin
   * @param frame_id_t frame_id. 被固定的frame
   * @return 无返回值
   */
  virtual void pin(frame_id_t frame_id) = 0;

  /**
   * Unpins a frame, indicating that it can now be victimized.
   * @param frame_id the id of the frame to unpin
   */
  /*@description: 取消对frame的固定，使得一个frame可以成为victim frame
   * @param frame_id_t frame_id
   *@return 无返回值
   */
  virtual void unpin(frame_id_t frame_id) = 0;

  /** @return the number of elements in the replacer that can be victimized */
  virtual size_t Size() = 0;
};
