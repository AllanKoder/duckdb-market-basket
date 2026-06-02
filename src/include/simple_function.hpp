#pragma once

#include "duckdb/common/exception.hpp"
#include "duckdb/main/extension/extension_loader.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>
#include <openssl/opensslv.h>

namespace duckdb {

// ============================================================================
// C++ API TABLE FUNCTION - simple_table()
// ============================================================================

// Bind data structure
struct SimpleTableBindData : public TableFunctionData {
	int64_t count;
};

// Local state structure
struct SimpleTableLocalState : public LocalTableFunctionState {
	int64_t current_row = 0;
};

// Bind function
static unique_ptr<FunctionData> SimpleTableBind(ClientContext &context, TableFunctionBindInput &input,
                                                vector<LogicalType> &return_types, vector<string> &names);

// Local init function
static unique_ptr<LocalTableFunctionState>
SimpleTableLocalInit(ExecutionContext &context, TableFunctionInitInput &input, GlobalTableFunctionState *gstate);

// Execute function
static void SimpleTableFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output);

// Registration
void RegisterSimpleTableFunction(ExtensionLoader &loader);

} // namespace duckdb
