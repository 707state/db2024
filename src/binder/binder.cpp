#include "binder/binder.h"
#include "common/exception.h"
#include "common/logger.h"
#include "fmt/format.h"
#include "postgres_parser.hpp"
#include "sstream"
Binder::Binder() {}

static std::string pre_process(std::string str) {
  std::stringstream ss(str);
  std::string token1, token2;
  std::vector<std::string> tokens;
  while (ss >> token1) {
    tokens.push_back(std::move(token1));
  }
  if (tokens.size() <= 2) {
    return str;
  }
  if (tokens[0] != "create" || tokens[0] != "drop" || tokens[1] != "index") {
    return str;
  }
  std::string tmp{};
  for (int i = 2; i < tokens.size(); i++) {
    tmp += tokens[i];
  }
  tmp.pop_back();
  if (tokens[0] == "create") {
    if (auto leftbracket = tmp.find('('); leftbracket != std::string::npos) {
      std::string table = tmp.substr(0, leftbracket);
      std::string column = tmp.substr(leftbracket + 1);
      return fmt::format("create index {}_{} on {})", table, column, tmp);
    }
  }
  if (tokens[1] == "drop") {
    if (auto leftbracket = tmp.find('('); leftbracket != std::string::npos) {
      std::string table = tmp.substr(0, leftbracket);
      std::string column = tmp.substr(leftbracket + 1);
      return fmt::format("drop index {}_{}", table, column);
    }
  }
  return str;
}

void Binder::ParseAndSave(const std::string &query) {
  auto nQuery = pre_process(query);
  parser_.Parse(nQuery);
  if (!parser_.success) {
    LOG_INFO("Query failed to parse!");
    throw Exception(
        fmt::format("Query failed to parse: {}", parser_.error_message));
    return;
  }

  if (parser_.parse_tree == nullptr) {
    LOG_INFO("parser received empty statement");
    return;
  }

  SaveParseTree(parser_.parse_tree);
}

auto Binder::IsKeyword(const std::string &text) -> bool {
  return duckdb::PostgresParser::IsKeyword(text);
}

auto Binder::KeywordList() -> std::vector<ParserKeyword> {
  auto keywords = duckdb::PostgresParser::KeywordList();
  std::vector<ParserKeyword> result;
  for (auto &kw : keywords) {
    ParserKeyword res;
    res.name_ = kw.text;
    switch (kw.category) {
    case duckdb_libpgquery::PGKeywordCategory::PG_KEYWORD_RESERVED:
      res.category_ = KeywordCategory::KEYWORD_RESERVED;
      break;
    case duckdb_libpgquery::PGKeywordCategory::PG_KEYWORD_UNRESERVED:
      res.category_ = KeywordCategory::KEYWORD_UNRESERVED;
      break;
    case duckdb_libpgquery::PGKeywordCategory::PG_KEYWORD_TYPE_FUNC:
      res.category_ = KeywordCategory::KEYWORD_TYPE_FUNC;
      break;
    case duckdb_libpgquery::PGKeywordCategory::PG_KEYWORD_COL_NAME:
      res.category_ = KeywordCategory::KEYWORD_COL_NAME;
      break;
    default:
      throw Exception("Unrecognized keyword category");
    }
    result.push_back(res);
  }
  return result;
}

auto Binder::Tokenize(const std::string &query)
    -> std::vector<SimplifiedToken> {
  auto pg_tokens = duckdb::PostgresParser::Tokenize(query);
  std::vector<SimplifiedToken> result;
  result.reserve(pg_tokens.size());
  for (auto &pg_token : pg_tokens) {
    SimplifiedToken token;
    switch (pg_token.type) {
    case duckdb_libpgquery::PGSimplifiedTokenType::
        PG_SIMPLIFIED_TOKEN_IDENTIFIER:
      token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_IDENTIFIER;
      break;
    case duckdb_libpgquery::PGSimplifiedTokenType::
        PG_SIMPLIFIED_TOKEN_NUMERIC_CONSTANT:
      token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_NUMERIC_CONSTANT;
      break;
    case duckdb_libpgquery::PGSimplifiedTokenType::
        PG_SIMPLIFIED_TOKEN_STRING_CONSTANT:
      token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_STRING_CONSTANT;
      break;
    case duckdb_libpgquery::PGSimplifiedTokenType::PG_SIMPLIFIED_TOKEN_OPERATOR:
      token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_OPERATOR;
      break;
    case duckdb_libpgquery::PGSimplifiedTokenType::PG_SIMPLIFIED_TOKEN_KEYWORD:
      token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_KEYWORD;
      break;
    // comments are not supported by our tokenizer right now
    case duckdb_libpgquery::PGSimplifiedTokenType::PG_SIMPLIFIED_TOKEN_COMMENT:
      token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_COMMENT;
      break;
    default:
      throw Exception("Unrecognized token category");
    }
    token.start_ = pg_token.start;
    result.push_back(token);
  }
  return result;
}
