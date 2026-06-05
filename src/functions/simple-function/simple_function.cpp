#include "duckdb/common/exception.hpp"
#include "duckdb/main/extension/extension_loader.hpp"
#include "simple_function.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>
#include "frequency_count.hpp"

#include <openssl/opensslv.h>

namespace duckdb {
// ============================================================================
// C++ API TABLE FUNCTION - simple_table()
// ============================================================================

// Bind function
static unique_ptr<FunctionData> SimpleTableBind(ClientContext &context, TableFunctionBindInput &input,
                                                vector<LogicalType> &return_types, vector<string> &names) {
	int64_t count = 10;
	if (!input.inputs.empty()) {
		count = input.inputs[0].GetValue<int64_t>();
	}
	auto result = make_uniq<SimpleTableBindData>();
	result->count = count;
	return_types = {LogicalType::BIGINT, LogicalType::VARCHAR};
	names = {"id", "value"};
	return std::move(result);
}

// Local init function
static unique_ptr<LocalTableFunctionState>
SimpleTableLocalInit(ExecutionContext &context, TableFunctionInitInput &input, GlobalTableFunctionState *gstate) {
	return make_uniq<SimpleTableLocalState>();
}

// Execute function
static void SimpleTableFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	auto &bind_data = data_p.bind_data->Cast<SimpleTableBindData>();
	auto &local_state = data_p.local_state->Cast<SimpleTableLocalState>();

	idx_t out_idx = 0;
	while (out_idx < STANDARD_VECTOR_SIZE && local_state.current_row < bind_data.count) {
		// Column 0: ID (BIGINT)
		auto id_data = FlatVector::GetData<int64_t>(output.data[0]);
		id_data[out_idx] = local_state.current_row;

		// Column 1: Value (VARCHAR)
		string value = "Row_" + to_string(local_state.current_row);
		string_t str_val = StringVector::AddString(output.data[1], value);
		auto str_data = FlatVector::GetData<string_t>(output.data[1]);
		str_data[out_idx] = str_val;

		local_state.current_row++;
		out_idx++;
	}
	utility::FrequencyCount<string> counter(9999999999);

	counter.AddElement("test");
	counter.AddElement("test");
	counter.AddElement("test");
	counter.AddElement("test");

	counter.AddElement("test1");

	std::cout << "Query 'test': " << counter.Query("test") << "\n";
	std::cout << "Query 'test1': " << counter.Query("test1") << "\n";
	std::cout << "Query 'test2': " << counter.Query("test2") << "\n";
	std::cout << "Query 'test3': " << counter.Query("test3") << "\n";

	output.SetCardinality(out_idx);
}

// Registration
void RegisterSimpleTableFunction(ExtensionLoader &loader) {
	// Version with BIGINT parameter
	TableFunction func("simple_table", {LogicalType::BIGINT}, SimpleTableFunction, SimpleTableBind, nullptr,
	                   SimpleTableLocalInit);
	loader.RegisterFunction(func);

	// Version without parameters (default to 10 rows)
	TableFunction func_no_params("simple_table", {}, SimpleTableFunction, SimpleTableBind, nullptr,
	                             SimpleTableLocalInit);
	loader.RegisterFunction(func_no_params);
}
} // namespace duckdb
