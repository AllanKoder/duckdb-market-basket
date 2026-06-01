#define DUCKDB_EXTENSION_MAIN

#include "market_basket_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/function/scalar_function.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

// OpenSSL linked through vcpkg
#include <openssl/opensslv.h>

namespace duckdb {

inline void MarketBasketScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &name_vector = args.data[0];
	UnaryExecutor::Execute<string_t, string_t>(name_vector, result, args.size(), [&](string_t name) {
		return StringVector::AddString(result, "...........🦆 " + name.GetString());
	});
}

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
static unique_ptr<LocalTableFunctionState> SimpleTableLocalInit(ExecutionContext &context,
                                                                 TableFunctionInitInput &input,
                                                                 GlobalTableFunctionState *gstate) {
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
	output.SetCardinality(out_idx);
}

// Registration
static void RegisterSimpleTableFunction(ExtensionLoader &loader) {
	// Version with BIGINT parameter
	TableFunction func("simple_table", {LogicalType::BIGINT}, SimpleTableFunction, SimpleTableBind,
	                   nullptr, SimpleTableLocalInit);
	loader.RegisterFunction(func);

	// Version without parameters (default to 10 rows)
	TableFunction func_no_params("simple_table", {}, SimpleTableFunction, SimpleTableBind,
	                              nullptr, SimpleTableLocalInit);
	loader.RegisterFunction(func_no_params);
}
inline void MarketBasketOpenSSLVersionScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
	auto &name_vector = args.data[0];
	UnaryExecutor::Execute<string_t, string_t>(name_vector, result, args.size(), [&](string_t name) {
		return StringVector::AddString(result, "MarketBasket " + name.GetString() + ", my linked OpenSSL version is " +
		                                           OPENSSL_VERSION_TEXT);
	});
}

static void LoadInternal(ExtensionLoader &loader) {
	// Register a scalar function
	auto market_basket_scalar_function =
	    ScalarFunction("market_basket", {LogicalType::VARCHAR}, LogicalType::VARCHAR, MarketBasketScalarFun);

	loader.RegisterFunction(market_basket_scalar_function);

	// Register another scalar function
	auto market_basket_openssl_version_scalar_function =
	    ScalarFunction("market_basket_openssl_version", {LogicalType::VARCHAR}, LogicalType::VARCHAR,
	                   MarketBasketOpenSSLVersionScalarFun);
	loader.RegisterFunction(market_basket_openssl_version_scalar_function);
}

void MarketBasketExtension::Load(ExtensionLoader &loader) {
	LoadInternal(loader);
	RegisterSimpleTableFunction(loader);
}
std::string MarketBasketExtension::Name() {
	return "market_basket";
}

std::string MarketBasketExtension::Version() const {
#ifdef EXT_VERSION_MARKET_BASKET
	return EXT_VERSION_MARKET_BASKET;
#else
	return "";
#endif
}

} // namespace duckdb

extern "C" {

DUCKDB_CPP_EXTENSION_ENTRY(market_basket, loader) {
	duckdb::LoadInternal(loader);
}
}
