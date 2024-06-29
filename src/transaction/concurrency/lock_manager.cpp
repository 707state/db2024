
/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL
v2. You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "lock_manager.h"

bool LockManager::lock_stmt_check(Transaction *txn) {
  // 检查事务状态 只有GROWING和DEFAULT可以加锁
  // 如果是DEFAULT状态 就把状态设置为GROWING
  //  TransactionState state_ 事务状态
  if (txn->get_state() == TransactionState::SHRINKING ||
      txn->get_state() == TransactionState::ABORTED ||
      txn->get_state() == TransactionState::COMMITTED) {
    return false;
  } else {
    txn->set_state(TransactionState::GROWING);
    return true;
  }
}

bool LockManager::unlock_stmt_check(Transaction *txn) {
  // 检查事务状态 只有GROWING DEFAULT阶段不能释放锁
  if (txn->get_state() == TransactionState::GROWING ||
      txn->get_state() == TransactionState::DEFAULT) {
    return false;
  } else {
    return true;
  }
}

/**
 * @description: 申请行级共享锁
 * @return {bool} 加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {Rid&} rid 加锁的目标记录ID 记录所在的表的fd
 * @param {int} tab_fd
 */
bool LockManager::lock_shared_on_record(Transaction *txn, const Rid &rid,
                                        int tab_fd) {
  // { DEFAULT, GROWING, SHRINKING, COMMITTED, ABORTED }
  // 要符合两阶段封锁协议，因此只能在GROWING阶段加锁
  std::scoped_lock lock(latch_);
  if (!lock_stmt_check(txn)) { // 判断该阶段能否加锁
    return false;
  }
  // 再根据LockID判断该事务是否已经加锁
  // 行级锁: LockDataId(int fd, const Rid &rid, LockDataType type)
  LockDataId lock_id(tab_fd, rid, LockDataType::RECORD);

  if (lock_table_.find(lock_id) == lock_table_.end()) {
    // 没找到就说明没有加锁，此时可以加锁
    // <LockDataId, LockRequestQueue> lock_table_
    LockRequest lock_request(txn->get_transaction_id(), LockMode::SHARED);
    lock_table_.insert(lock_id);
    lock_table_.at(lock_id).request_queue_.push_back(lock_request);
    return true;
  } else {
    // 如果已经有锁了
    return false;
  }
}

/**
 * @description: 申请行级排他锁
 * @return {bool} 加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {Rid&} rid 加锁的目标记录ID
 * @param {int} tab_fd 记录所在的表的fd
 */
bool LockManager::lock_exclusive_on_record(Transaction *txn, const Rid &rid,
                                           int tab_fd) {
  std::scoped_lock lock(latch_);
  if (!lock_stmt_check(txn)) { // 判断该阶段能否加锁
    return false;
  }
  LockDataId lock_id(tab_fd, rid, LockDataType::RECORD);

  if (lock_table_.find(lock_id) == lock_table_.end()) {
    LockRequest lock_request(txn->get_transaction_id(), LockMode::EXLUCSIVE);
    lock_table_.try_emplace(lock_id);
    lock_table_.at(lock_id).request_queue_.push_back(lock_request);
    return true;
  } else {
    // 如果已经有锁了 判断是什么类型的锁 S锁可以升级成X锁
    auto &lock_requests = lock_table_[lock_id].request_queue_;
    for (auto &lock_rq : lock_requests) {
      if (lock_rq.lock_mode_ == LockMode::SHARED) {
        lock_rq.lock_mode_ = LockMode::EXLUCSIVE;
        return true;
      } else {
        return false;
      }
    }
  }
}

/**
 * @description: 申请表级读锁
 * @return {bool} 返回加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {int} tab_fd 目标表的fd
 */
bool LockManager::lock_shared_on_table(Transaction *txn, int tab_fd) {
  std::scoped_lock lock(latch_);
  if (!lock_stmt_check(txn)) { // 判断该阶段能否加锁
    return false;
  }
  LockDataId lock_id(tab_fd, LockDataType::RECORD);

  if (lock_table_.find(lock_id) == lock_table_.end()) {
    // 没找到就说明没有加锁，此时可以加锁
    // <LockDataId, LockRequestQueue> lock_table_
    LockRequest lock_request(txn->get_transaction_id(), LockMode::SHARED);
    lock_table_.try_emplace(lock_id);
    lock_table_.at(lock_id).request_queue_.push_back(lock_request);
    return true;
  } else {
    // 如果已经有锁了
    return false;
  }
}

/**
 * @description: 申请表级写锁
 * @return {bool} 返回加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {int} tab_fd 目标表的fd
 */
bool LockManager::lock_exclusive_on_table(Transaction *txn, int tab_fd) {
  std::scoped_lock lock(latch_);
  if (!lock_stmt_check(txn)) { // 判断该阶段能否加锁
    return false;
  }
  LockDataId lock_id(tab_fd, LockDataType::RECORD);

  if (lock_table_.find(lock_id) == lock_table_.end()) {
    LockRequest lock_request(txn->get_transaction_id(), LockMode::EXLUCSIVE);
    lock_table_.try_emplace(lock_id);
    lock_table_.at(lock_id).request_queue_.push_back(lock_request);
    return true;
  } else {
    // 如果已经有锁了 判断是什么类型的锁 S锁可以升级成X锁
    auto &lock_requests = lock_table_[lock_id].request_queue_;
    for (auto &lock_rq : lock_requests) {
      if (lock_rq.lock_mode_ == LockMode::SHARED) {
        lock_rq.lock_mode_ = LockMode::EXLUCSIVE;
        return true;
      } else {
        return false;
      }
    }
  }
  return true;
}

/**
 * @description: 申请表级意向读锁
 * @return {bool} 返回加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {int} tab_fd 目标表的fd
 */
bool LockManager::lock_IS_on_table(Transaction *txn, int tab_fd) {
  std::scoped_lock lock(latch_);
  if (!lock_stmt_check(txn)) { // 判断该阶段能否加锁
    return false;
  }
  LockDataId lock_id(tab_fd, LockDataType::RECORD);

  if (lock_table_.find(lock_id) == lock_table_.end()) {
    // 没找到就说明没有加锁，此时可以加锁
    // <LockDataId, LockRequestQueue> lock_table_
    LockRequest lock_request(txn->get_transaction_id(),
                             LockMode::INTENTION_SHARED);
    lock_table_.try_emplace(lock_id);
    lock_table_.at(lock_id).request_queue_.push_back(lock_request);
    return true;
  } else {
    // 如果已经有锁了
    return false;
  }
}

/**
 * @description: 申请表级意向写锁
 * @return {bool} 返回加锁是否成功
 * @param {Transaction*} txn 要申请锁的事务对象指针
 * @param {int} tab_fd 目标表的fd
 */
bool LockManager::lock_IX_on_table(Transaction *txn, int tab_fd) {
  std::scoped_lock lock(latch_);
  if (!lock_stmt_check(txn)) { // 判断该阶段能否加锁
    return false;
  }
  LockDataId lock_id(tab_fd, LockDataType::RECORD);

  if (lock_table_.find(lock_id) == lock_table_.end()) {
    LockRequest lock_request(txn->get_transaction_id(),
                             LockMode::INTENTION_EXCLUSIVE);
    lock_table_.try_emplace(lock_id);
    lock_table_.at(lock_id).request_queue_.push_back(lock_request);
    return true;
  } else {
    // 如果已经有锁了
    return false;
  }
}

/**
 * @description: 释放锁
 * @return {bool} 返回解锁是否成功
 * @param {Transaction*} txn 要释放锁的事务对象指针
 * @param {LockDataId} lock_data_id 要释放的锁ID
 */
bool LockManager::unlock(Transaction *txn, LockDataId lock_data_id) {
  std::scoped_lock lock(latch_);
  if (!unlock_stmt_check(txn)) {
    // 检查是否可以释放锁 可以释放返回true 不可释放返回false
    return false;
  }
  if (lock_table_.find(lock_data_id) == lock_table_.end()) {
    // 没有找到该lock_id对应的锁 即代表锁已经被释放了 或者没有被上锁
    return true;
  }
  auto &lock_requests = lock_table_[lock_data_id].request_queue_;
  for (auto it = lock_requests.begin(); it != lock_requests.end();) {
    if (it->txn_id_ == txn->get_transaction_id()) {
      it = lock_requests.erase(it);
    } else {
      ++it;
    }
  }
  if (lock_requests.empty()) {
    lock_table_.erase(lock_data_id);
  }
  return true;
}
