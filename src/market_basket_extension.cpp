#define DUCKDB_EXTENSION_MAIN

#include "market_basket_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/function/table_function.hpp"
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
// 1. BIND DATA - Stores information from binding phase
// ============================================================================
struct SimpleTableBindData : public TableFunctionData {
	explicit SimpleTableBindData(const vector<Value> &inputs) {
		if (inputs.size() > 0 && !inputs[0].IsNull()) {
			count = inputs[0].GetValue<int64_t>();
		} else {
			count = 10; // default
		}
	}
	int64_t count;
};

// ============================================================================
// 2. LOCAL STATE - Per-thread execution context
// ============================================================================
struct SimpleTableLocalState : public LocalTableFunctionState {
	int64_t current_row = 0;
};

// ============================================================================
// 3. BIND FUNCTION - Called once during query planning
// ============================================================================
static unique_ptr<FunctionData> SimpleTableBind(ClientContext &context, TableFunctionBindInput &input,
                                                vector<LogicalType> &return_types, vector<string> &names) {
	// Define output schema (2 columns)
	return_types.push_back(LogicalType::BIGINT);
	names.push_back("id");

	return_types.push_back(LogicalType::VARCHAR);
	names.push_back("value");

	// Store bind data
	return make_uniq<SimpleTableBindData>(input.inputs);
}

// ============================================================================
// 4. LOCAL INIT FUNCTION - Initialize per-thread state
// ============================================================================
static unique_ptr<LocalTableFunctionState>
SimpleTableLocalInit(ExecutionContext &context, TableFunctionInitInput &input, GlobalTableFunctionState *global_state) {
	return make_uniq<SimpleTableLocalState>();
}

// ============================================================================
// 5. EXECUTE FUNCTION - Generate output rows
// ============================================================================
static void SimpleTableFunction(ClientContext &context, TableFunctionInput &data_p, DataChunk &output) {
	// Get state objects
	auto &bind_data = data_p.bind_data->Cast<SimpleTableBindData>();
	auto &local_state = data_p.local_state->Cast<SimpleTableLocalState>();

	// Generate rows until we reach the requested count
	idx_t out_idx = 0;
	while (out_idx < STANDARD_VECTOR_SIZE && local_state.current_row < bind_data.count) {
		// Column 0: ID (int64)
		FlatVector::GetData<int64_t>(output.data[0])[out_idx] = local_state.current_row;

		// Column 1: Value (string) - use AddString to create the string
		string value = "Row_" + to_string(local_state.current_row);
		FlatVector::GetData<string_t>(output.data[1])[out_idx] = StringVector::AddString(output.data[1], value);

		local_state.current_row++;
		out_idx++;
	}

	output.SetCardinality(out_idx);
}

// ============================================================================
// 7. REGISTER - Add to extension
// ============================================================================
void RegisterSimpleTableFunction(ExtensionLoader &loader) {
	TableFunction table_func("simple_table",        // function name
	                         {LogicalType::BIGINT}, // parameter types
	                         SimpleTableFunction,   // execute function
	                         SimpleTableBind,       // bind function
	                         nullptr,               // init_global (optional)
	                         SimpleTableLocalInit); // init_local (optional)

	// Register with parameter (simple_table(n))
	loader.RegisterFunction(table_func);

	// Register without parameters (simple_table()) - uses default count=10
	TableFunction table_func_default("simple_table",        // function name
	                                 {},                    // no parameters
	                                 SimpleTableFunction,   // execute function
	                                 SimpleTableBind,       // bind function
	                                 nullptr,               // init_global (optional)
	                                 SimpleTableLocalInit); // init_local (optional)

	loader.RegisterFunction(table_func_default);
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
