#include "execution/executor_insert.h"
#include "record/rm_defs.h"
#include "type/type_id.h"
#include <memory>
std::unique_ptr<RmRecord> InsertExecutor::Next() {
  // Make record buffer
  RmRecord rec(fh_->get_file_hdr().record_size);
  for (size_t i = 0; i < values_.size(); i++) {
    auto &col = tab_.cols[i];
    auto &val = values_[i];
    if (col.type != coltype_to_typeid(val.type)) {
      throw IncompatibleTypeError(typeid2str(col.type), coltype2str(val.type));
    }
    val.init_raw(col.len);
    memcpy(rec.data + col.offset, val.raw->data, col.len);
  }
  // Insert into record file
  rid_ = fh_->insert_record(rec.data, context_);

  // Insert into index
  for (size_t i = 0; i < tab_.indexes.size(); ++i) {
    auto &index = tab_.indexes[i];
    auto ih = sm_manager_->ihs_
                  .at(sm_manager_->get_ix_manager()->get_index_name(tab_name_,
                                                                    index.cols))
                  .get();
    char *key = new char[index.col_tot_len];
    int offset = 0;
    for (size_t i = 0; i < index.col_num; ++i) {
      memcpy(key + offset, rec.data + index.cols[i].offset, index.cols[i].len);
      offset += index.cols[i].len;
    }
    ih->insert_entry(key, rid_, context_->txn_);
  }
  return nullptr;
}
